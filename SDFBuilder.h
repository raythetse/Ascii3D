#pragma once
#include "SDFObject.h"
#include "SDFOperation.h"

class SDFBuilder {
private:
    std::unique_ptr<SDFObject> m_baseObject;
    std::vector<SDFObject*> m_objChain;
    SDFOperation* m_lastOp{};
public:
    SDFBuilder(Vec3 position = Vec3::Zero, Vec3 rotation = Vec3::Zero, Color color = Color::White, bool lit = true);
    std::unique_ptr<SDFObject> get();
	template <std::derived_from<SDFOperation> T, typename... Args>
    SDFBuilder& pushOp(Args&&... args);
    template <std::derived_from<SDFOperation> T, std::derived_from<SDFObject> U, typename... ArgsT>
    auto pushOp(ArgsT&&... argst);
    SDFBuilder& stepBack(size_t steps = 1);
    SDFBuilder& color(Vec3 color);
    template <typename T>
        requires (!std::is_base_of_v<PropertyModifierBase, T>)
    SDFBuilder& bindPropMod(std::unique_ptr<PropertyModifier<T>> propMod);
    template <typename T, typename... Args>
        requires (std::is_base_of_v<PropertyModifierBase, T>)
    SDFBuilder& bindPropMod(Args&&... args);
};

template <std::derived_from<SDFOperation> T, typename... Args>
SDFBuilder& SDFBuilder::pushOp(Args&&... args) {
    m_lastOp = m_objChain.back()->pushOperation<T>(std::forward<Args>(args)...);
    return *this;
}

template <std::derived_from<SDFOperation> T, std::derived_from<SDFObject> U, typename... ArgsT>
auto SDFBuilder::pushOp(ArgsT&&... argst) {
    return [this, ...captured_args = std::forward<ArgsT>(argst)](auto&&... argsu) mutable -> auto& {
        auto obj = std::make_unique<U>(std::move(captured_args)...);
        auto prev = m_objChain.back();
        m_objChain.push_back(obj.get());
        auto op = std::make_unique<T>(std::move(obj), std::forward<decltype(argsu)>(argsu)...);
        m_lastOp = op.get();
        prev->pushOperation(std::move(op));

        return *this;
    };
}

template <typename T>
    requires (!std::is_base_of_v<PropertyModifierBase, T>)
SDFBuilder& SDFBuilder::bindPropMod(std::unique_ptr<PropertyModifier<T>> propMod)
{
    if (m_lastOp) {
        m_lastOp->bindPropertyModifier(std::move(propMod));
    }
    return *this;
}

template <typename T, typename... Args>
    requires (std::is_base_of_v<PropertyModifierBase, T>)
SDFBuilder& SDFBuilder::bindPropMod(Args&&... args) {
    if (m_lastOp) {
        m_lastOp->bindPropertyModifier(std::move(std::make_unique<T>(std::forward<Args>(args)...)));
    }
    return *this;
}