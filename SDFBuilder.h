#pragma once
#include <functional>
#include <sstream>
#include <unordered_map>
#include "LightSource.h"
#include "SDFObject.h"
#include "SDFOperation.h"
#include "SDFOperationInc.h"

class SDFScene;

class SDFBuilder {
    static const std::unordered_map<std::string, std::function<void(SDFBuilder& builder, std::stringstream&)>> s_opMap;
private:
    std::unique_ptr<SDFObject> m_baseObject;
    std::vector<SDFObject*> m_objChain;
    std::vector<std::unique_ptr<LightSource>> m_lights;
    SDFOperation* m_lastOp{};
    template<typename T, typename... Args>
        requires (std::is_base_of_v<SDFObject, T> || std::same_as<SDFObject, T>)
    SDFBuilder& pushSDFObj(Args&&... args);
public:
    SDFBuilder();
    std::unique_ptr<SDFObject> getBaseObject();
    std::vector<std::unique_ptr<LightSource>> getLights();
	template <std::derived_from<SDFOperation> T, typename... Args>
    SDFBuilder& pushOp(Args&&... args);
    template <std::derived_from<SDFOperation> T, std::derived_from<SDFObject> U, typename... ArgsT>
    auto pushOp(ArgsT&&... argst);
    SDFBuilder& addLightSource(Vec3 position = LightSource::DFLT_POSITION, double luminance = LightSource::DFLT_LUMINANCE, Color color = LightSource::DFLT_COLOR, double ambientPower = LightSource::DFLT_AMBIENTPOWER);
    SDFBuilder& stepBack(size_t steps = 1);
    SDFBuilder& color(Color color);
    template <typename T>
        requires (!std::is_base_of_v<PropertyModifierBase, T>)
    SDFBuilder& bindPropMod(std::unique_ptr<PropertyModifier<T>> propMod);
    template <typename T, typename... Args>
        requires (std::is_base_of_v<PropertyModifierBase, T>)
    SDFBuilder& bindPropMod(Args&&... args);
    void loadSerialized(std::vector<std::string>& strVec);
};

template<typename T, typename ...Args>
    requires (std::is_base_of_v<SDFObject, T> || std::same_as<SDFObject, T>)
SDFBuilder& SDFBuilder::pushSDFObj(Args && ...args)
{
    auto obj = std::make_unique<T>(std::forward<Args>(args)...);
    m_objChain.push_back(obj.get());
    dynamic_cast<CombinationOperation*>(m_lastOp)->setCombineObject(std::move(obj));
    return *this;
}

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