#pragma once
#include "SceneObject.h"

class SOBuilder {
private:
    std::unique_ptr<SceneObject> m_baseObject;
    std::vector<SceneObject*> m_objChain;
public:
    SOBuilder(Vec3 position = Vec3::Zero, Vec3 rotation = Vec3::Zero, Color color = Color::White, bool lit = true);
    std::unique_ptr<SceneObject> get();
	template <std::derived_from<SDFOperation> T, typename... Args>
    SOBuilder& pushOp(Args&&... args);
    template <std::derived_from<SDFOperation> T, std::derived_from<SceneObject> U, typename... ArgsT>
    auto pushOp(ArgsT&&... argst);
    SOBuilder& back(size_t steps = 1);
    SOBuilder& color(Vec3 color);
};

template <std::derived_from<SDFOperation> T, typename... Args>
SOBuilder& SOBuilder::pushOp(Args&&... args) {
    m_objChain.back()->pushOperation<T>(std::forward<Args>(args)...);
    return *this;
}

template <std::derived_from<SDFOperation> T, std::derived_from<SceneObject> U, typename... ArgsT>
auto SOBuilder::pushOp(ArgsT&&... argst) {
    return [this, ...captured_args = std::forward<ArgsT>(argst)](auto&&... argsu) mutable -> auto& {
        auto obj = std::make_unique<U>(std::move(captured_args)...);
        auto prev = m_objChain.back();
        m_objChain.push_back(obj.get());
        auto op = std::make_unique<T>(std::move(obj), std::forward<decltype(argsu)>(argsu)...);

        prev->pushOperation(std::move(op));

        return *this;
    };
}