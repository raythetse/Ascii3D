#pragma once
#include "SDFObject.h"

class SDFBuilder {
private:
    std::unique_ptr<SDFObject> m_baseObject;
    std::vector<SDFObject*> m_objChain;
public:
    SDFBuilder(Vec3 position = Vec3::Zero, Vec3 rotation = Vec3::Zero, Color color = Color::White, bool lit = true);
    std::unique_ptr<SDFObject> get();
	template <std::derived_from<SDFOperation> T, typename... Args>
    SDFBuilder& pushOp(Args&&... args);
    template <std::derived_from<SDFOperation> T, std::derived_from<SDFObject> U, typename... ArgsT>
    auto pushOp(ArgsT&&... argst);
    SDFBuilder& back(size_t steps = 1);
    SDFBuilder& color(Vec3 color);
};

template <std::derived_from<SDFOperation> T, typename... Args>
SDFBuilder& SDFBuilder::pushOp(Args&&... args) {
    m_objChain.back()->pushOperation<T>(std::forward<Args>(args)...);
    return *this;
}

template <std::derived_from<SDFOperation> T, std::derived_from<SDFObject> U, typename... ArgsT>
auto SDFBuilder::pushOp(ArgsT&&... argst) {
    return [this, ...captured_args = std::forward<ArgsT>(argst)](auto&&... argsu) mutable -> auto& {
        auto obj = std::make_unique<U>(std::move(captured_args)...);
        auto prev = m_objChain.back();
        m_objChain.push_back(obj.get());
        auto op = std::make_unique<T>(std::move(obj), std::forward<decltype(argsu)>(argsu)...);

        prev->pushOperation(std::move(op));

        return *this;
    };
}