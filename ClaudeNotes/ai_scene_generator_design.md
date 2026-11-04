# AI scene generator: design note (DRAFT)

Draft 2026-11-04, for review. Nothing here is implemented.

The idea: the user types a scene description in the app, an integrated Claude
client writes a `.scn` file for it, and the app renders the result.

Most of the groundwork exists already:
- `scene_format_spec_v0.4.md` was written as this prompt's spec.
- `SceneValidator`, inside the app, has generator-only rules.
- The loader warns about malformed scenes and recovers.
- The warnings screen shows those warnings before rendering.

This note covers what's still missing: where the feature sits in the app,
how it talks to the API, the prompt, the validate-and-retry loop, and what
happens when things fail.

**Scope check.** This is feature work, not the "portfolio polish only"
from the handoff ground rules. It is self-contained (new files, plus one menu
entry), and it could be the most interesting thing in a portfolio README.
Whether to build it, and when, is your call.

## 1. User flow

1. The scene-type menu (`GameEngine.cpp:173`: `[1] Built-In`, `[2] Custom`,
   `[3] Back`) gains `[3] Generate from description`. Back becomes `[4]`.
2. The user types a description on one line, e.g. `a snowman next to a lamp
   post at night`. An empty line goes back.
3. The app shows `Generating...`. A request takes tens of seconds, so the
   screen should show that it is working.
4. The reply is checked (Section 4). If it fails, the problems are sent back
   for up to two retries. The screen shows `Fixing 3 problems (attempt 2/3)...`.
5. The result is saved to `CustomScenes/Generated_<slug>.scn`. The slug comes
   from the description, with a number added if the name is taken. The file
   starts with a `//` comment holding the description, so each file records
   what it was asked for.
6. The file then loads through the existing custom-scene path:
   `SDFScene{name}`, then `confirmLoadWarnings`, then render. So a scene that
   still has problems after the retries shows the existing warnings screen,
   with `[ENTER]` to render anyway and `[B]` to go back.

Saving before rendering means a good result is never lost, and it can be
edited by hand and reloaded from `[2] Custom`.

## 2. Talking to the API

**There is no official C++ SDK, so the app calls the HTTP API directly:**
`POST https://api.anthropic.com/v1/messages`, with these headers:
- `x-api-key`
- `anthropic-version: 2026-10-21`
- `content-type: application/json`

**HTTP: WinHTTP.** It ships with Windows, so there's no new dependency, and
the project is Windows-only already (console API, MSBuild). It needs
`winhttp.lib` added to `AdditionalDependencies`. The alternative is libcurl,
which would only be worth it if a CMake or cross-platform build happens.

**JSON: nlohmann/json**, a single header vendored into the repo (or added
through vcpkg). Escaping a 25 KB spec by hand, and parsing the response by
hand, is where bugs would hide. This is the only new third-party code.

**Request shape (v1):**
- `model`: `claude-opus-5-5` by default, read from a config value so it can
  be changed without rebuilding. Cheaper models (`claude-sonnet-5-5`,
  `claude-haiku-5-5`) are a cost choice to try in Phase 0, not something to
  assume.
- `max_tokens`: 16000, non-streaming. Opus 5.5 always thinks, so the scene
  arrives after its reasoning. Streaming would allow a live progress display,
  but it means parsing server-sent events. Leave it for later.
- `output_config.effort`: start at `medium` (the Opus 5.5 default), and
  measure `high` in Phase 0.
- `output_config.format`: structured output with a JSON schema of
  `{ "scn": string, "notes": string }`. This makes extraction reliable: no
  hunting for a fenced code block in free text. `notes` holds the model's
  short explanation, such as "approximated the lamp's glow with a second
  light", which can be shown on the result screen.
- `system`: the prompt from Section 3, with `cache_control` on it. The prompt
  is large and identical on every call, so retries and later generations read
  it from the cache at a fraction of the price.
- Refusal handling: check `stop_reason` before reading content. Add the
  server-side `fallbacks: "default"` parameter, with its beta header
  `server-side-fallback-2026-07-01`. A scene description is unlikely to be
  declined, but if one is, it should fail cleanly.

**The key.** Read it from the `ANTHROPIC_API_KEY` environment variable. It is
never written to a file, a log or the console. If it's missing, the menu entry
explains how to set it, then goes back. A key typed into the app is a later
option, and only if it's stored somewhere sensible (Windows Credential
Manager), never in the repo.

**Code layout:** two new files, nothing else restructured.
- `ClaudeClient.h/.cpp`: one function. It takes the system prompt, the
  messages and the options. It returns the parsed reply or a typed error:
  missing key, network, HTTP status, refusal, or truncated.
- `SceneGenerator.h/.cpp`: the prompt, the retry loop (Section 4) and saving.
  It doesn't touch the console, so it can be tested without the menu.

`GameEngine` gets the menu entry and the progress text. This mirrors how
`SDFBuilder` collects warnings and `GameEngine` shows them.

## 3. The prompt

The system prompt is built from the spec, not written separately, so the two
can't drift apart:
- **Spec Sections 1-10.** These hold the format rules, the shapes and their
  parameters, the stack model and the worked examples. Sections 11 (validator
  rules) and the appendices (loader internals, changelog) are left out: the
  model doesn't need them, and they cost tokens on every call.
- **One or two trusted example scenes.** `Jack_O_Lantern.scn` is already
  marked as safe. A second one, in a different style, would help the model
  generalise.
- **A short instruction block:**
  - Return a complete file that follows the strict subset.
  - State every parameter.
  - Use at least one light.
  - Keep `OPCubeRollHeight`/`OPCubeRoller` out.
  - Approximate what the shapes can't express, and say so in `notes`.

Two options for where the prompt lives:
- **(a) A file**, for example `Prompts/scene_generator.md`, generated from the
  spec by a small script and read at runtime. It can be tuned without a
  rebuild, but it has to ship next to the exe.
- **(b) Embedded** in the exe at build time. That is simpler to ship, but
  every prompt change needs a rebuild.

I'd start with (a) while tuning, then decide.

Size: Sections 1-10 are about 18 KB, and Jack is 3.6 KB, so roughly 6-7k
tokens of system prompt. That is well within caching's minimum and the
context window.

## 4. Validate and retry

The spec already gives the rule: **a file that makes the loader warn is
invalid.** The generator applies the stricter version: a file with any
validator finding, errors or warnings, is sent back.

**Use `SceneValidator::validate` (in-process).** The app already runs it on
every custom scene, so the generator needs no extra dependency. Each finding carries a line number and a rule number, which is
exactly the feedback a retry needs. The warnings matter here, even though the
app doesn't show them for hand-written scenes: rule 9 (generator-excluded
keywords, every parameter written explicitly) exists for generated files.

**Retry:** append the model's reply and a user message that lists the
findings ("Fix these problems and return the whole file again:" followed by
the `Line N: ...` list) to the same conversation. Allow up to 2 retries, then
stop.

The conversation must only ever be appended to. Earlier turns are never
edited, which current models require for their thinking blocks, and which
also keeps the cache valid.

## 5. Failure modes

| Failure | Behaviour |
|---|---|
| No `ANTHROPIC_API_KEY` | Explain how to set it; back to the menu. No request is sent. |
| No network / DNS / TLS error | `Could not reach the API: <reason>`; back to the menu. |
| 401 / 403 | `The API key was rejected.` Never print the key. |
| 429 / 5xx / overloaded | Retry twice with backoff, honouring `retry-after`; then give up with the status. |
| `stop_reason: refusal` (after fallbacks) | `The request was declined.`, with the category if present; back to the menu. |
| `stop_reason: max_tokens` | Treat it as a failed attempt and retry with a "be more compact" note; if it repeats, give up. |
| Still invalid after retries | Save it anyway (`Generated_<slug>.scn`) and show the warnings screen as for any custom scene. |
| Reply isn't valid JSON or lacks `scn` | Count it as a failed attempt. |
| User wants to cancel a slow request | v1: no cancel, but a request timeout of a few minutes. A cancel key needs the request on a worker thread. |

## 6. Cost

Rough figures for Opus 5.5 ($4 input / $20 output per million tokens; $0.20
per million for cache reads):
- The first call is about 7k input tokens (about $0.03), plus 3-10k output
  tokens including thinking (about $0.06-$0.20).
- A retry mostly reads the prompt from the cache, so it costs mainly its
  output.

That puts one scene at **roughly $0.10-$0.30**. These figures are estimates;
Phase 0 measures them from `usage` in real responses.

## 7. Phases

0. **Prompt test, no new C++.** Build the system prompt from the spec. Run
   10-20 varied descriptions through Claude (a Claude Code session costs
   nothing extra), and check every result with `Ascii3D.exe --validate`. Then render the best
   ones in the app. This settles which model, which effort level, whether the
   examples are enough, and the real cost, before any app code exists. If the
   output is poor, the feature waits for a better spec, not better C++.
1. **`ClaudeClient` + `SceneGenerator`**, with no menu. Drive them from a
   small test harness, like the loader harness.
2. **The menu entry**, the progress text and the result screen showing
   `notes`.
3. **Later, if wanted:**
   - streaming progress
   - a cancel key
   - a key stored in Credential Manager
   - "refine this scene" (send the current file plus a change request)

## Open questions for you

1. Build this now, or after the README and tests? (Scope check above.)
2. Is a one-line description enough, or do you want a multi-line input?
3. Should generated files go in `CustomScenes/` with a `Generated_` prefix, or
   in a separate `GeneratedScenes/` folder?
4. Are you OK with nlohmann/json as a vendored header?
5. Should the model be fixed, or chosen per run? (Opus by default; Phase 0
   shows whether Sonnet or Haiku is good enough.)
6. (Settled: the validator is C++ and inside the app, so there is no Python
   dependency.)
