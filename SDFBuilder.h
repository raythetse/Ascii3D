#pragma once
#include <concepts>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include "Animation.h"
#include "LightSource.h"
#include "SDFObjectInc.h"
#include "SDFOperation.h"
#include "SDFOperationInc.h"

class SDFScene;

// Builds out a scene with a typical builder pattern allowing functions to be chained on a single extended statement.
// Makes heavy templating use to keep code repetition to a minimum though this unfortunately can bog down VS Intellisense.
class SDFBuilder {
    static const std::unordered_map<std::string, std::function<void(SDFBuilder& builder, std::stringstream&)>> s_opMap;
private:
    std::unique_ptr<SDFObject> m_baseObject;
    std::vector<SDFObject*> m_objChain;
    std::vector<std::unique_ptr<LightSource>> m_lights;
    SDFOperation* m_lastOp{};
    template <std::derived_from<SDFObject> T, typename... Args>
    SDFBuilder& pushSDFObj(Args&&... args);
    bool m_lastAddedObj{ true };

    // Loader diagnostics and recovery (used by loadSerialized).
    size_t m_lineNo{};      // 1-based line being loaded; 0 outside loadSerialized
    size_t m_skipDepth{};   // > 0 while skipping the subtree of a rejected shape
    std::vector<std::string> m_warnings;   // collected, not printed: rendering overwrites the console
    void warn(std::string_view msg);
    void discardPendingCombination();
    template <typename E>
    E toEnum(int value, E maxValue, E dflt, std::string_view name);
    size_t toCount(long long value, size_t dflt, std::string_view name);
    Vec3 toDirection(Vec3 value, Vec3 dflt, std::string_view name);
public:
    SDFBuilder();
    std::unique_ptr<SDFObject> getBaseObject();
    std::vector<std::unique_ptr<LightSource>> getLights();
    std::vector<std::string> getWarnings();
	template <std::derived_from<SDFOperation> T, typename... Args>
    SDFBuilder& operate(Args&&... args);
    template <std::derived_from<CombinationOperation> T, std::derived_from<SDFObject> U, typename... ArgsT>
    auto combine(ArgsT&&... argst);
    SDFBuilder& addLightSource(Vec3 position = LightSource::DFLT_POSITION, double luminance = LightSource::DFLT_LUMINANCE, Color color = LightSource::DFLT_COLOR, double ambientPower = LightSource::DFLT_AMBIENTPOWER);
    SDFBuilder& stepBack(size_t steps = 1);
    SDFBuilder& color(Color color);
    template <typename T, typename... Args>
        requires (std::is_base_of_v<Animation, T>)
    SDFBuilder& animate(Args&&... args);
    template <typename T, typename... Args>
        requires (std::is_base_of_v<Animation, T>)
    SDFBuilder& animateComb(Args&&... args);
    SDFBuilder& loadSerialized(std::vector<std::string>& strVec);
};

template <std::derived_from<SDFObject> T, typename... Args>
SDFBuilder& SDFBuilder::pushSDFObj(Args&&... args)
{
    auto* comb = dynamic_cast<CombinationOperation*>(m_lastOp);
    if (!comb) {
        warn("shape has no preceding combination; shape and its children skipped");
        m_skipDepth = 1;     // skip lines until this shape's own stepBack
        return *this;
    }
    auto obj = std::make_unique<T>(std::forward<Args>(args)...);
    m_objChain.push_back(obj.get());
    comb->setCombineObject(std::move(obj));
    m_lastOp = nullptr;          // a combination takes exactly one shape
    m_lastAddedObj = true;
    return *this;
}

template <std::derived_from<SDFOperation> T, typename... Args>
SDFBuilder& SDFBuilder::operate(Args&&... args) {
    m_lastOp = m_objChain.back()->pushOperation<T>(std::forward<Args>(args)...);
    m_lastAddedObj = false;
    return *this;
}

template <std::derived_from<CombinationOperation> T, std::derived_from<SDFObject> U, typename... ArgsT>
auto SDFBuilder::combine(ArgsT&&... argst) {
    return [this, ...captured_args = std::forward<ArgsT>(argst)](auto&&... argsu) mutable -> auto& {
        auto obj = std::make_unique<U>(std::move(captured_args)...);
        auto prev = m_objChain.back();
        m_objChain.push_back(obj.get());
        auto op = std::make_unique<T>(std::move(obj), std::forward<decltype(argsu)>(argsu)...);
        m_lastOp = op.get();
        prev->pushOperation(std::move(op));
        m_lastAddedObj = true;
        return *this;
    };
}

template <typename T, typename... Args>
    requires (std::is_base_of_v<Animation, T>)
SDFBuilder& SDFBuilder::animate(Args&&... args) {
    if (m_lastAddedObj) {
        m_objChain.back()->bindAnimation(std::move(std::make_unique<T>(std::forward<Args>(args)...)));
    }
    else if (m_lastOp) {
        m_lastOp->bindAnimation(std::move(std::make_unique<T>(std::forward<Args>(args)...)));
    }
    else {
        warn("animation has no target (after stepBack, LightSource or line 1); dropped");
    }
    return *this;
}

template <typename T, typename... Args>
    requires (std::is_base_of_v<Animation, T>)
SDFBuilder& SDFBuilder::animateComb(Args&&... args) {
    CombinationOperation* combOp = dynamic_cast<CombinationOperation*>(m_lastOp);
    if (combOp) {
        combOp->bindAnimation(std::move(std::make_unique<T>(std::forward<Args>(args)...)));
    }
    return *this;
}