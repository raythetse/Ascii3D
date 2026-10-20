#pragma once
#include <functional>
#include <sstream>
#include <unordered_map>
#include "Animation.h"
#include "LightSource.h"
#include "SDFObjectInc.h"
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
    bool m_lastAddedObj{ true };
public:
    SDFBuilder();
    std::unique_ptr<SDFObject> getBaseObject();
    std::vector<std::unique_ptr<LightSource>> getLights();
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

template<typename T, typename ...Args>
    requires (std::is_base_of_v<SDFObject, T> || std::same_as<SDFObject, T>)
SDFBuilder& SDFBuilder::pushSDFObj(Args && ...args)
{
    auto obj = std::make_unique<T>(std::forward<Args>(args)...);
    m_objChain.push_back(obj.get());
    dynamic_cast<CombinationOperation*>(m_lastOp)->setCombineObject(std::move(obj));
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