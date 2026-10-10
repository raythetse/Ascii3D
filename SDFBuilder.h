#pragma once
#include <functional>
#include <sstream>
#include <unordered_map>
#include "LightSource.h"
#include "SDFObjectInc.h"
#include "SDFOperation.h"
#include "SDFOperationInc.h"

class SDFScene;

class SDFBuilder {
    static const std::unordered_map<std::string, std::function<void(SDFBuilder& builder, std::stringstream&)>> s_opMap;
private:
    std::unique_ptr<std::unordered_map <std::string, std::vector<std::string>>> m_branchMap;
    std::unique_ptr<SDFObject> m_baseObject;
    std::vector<SDFObject*> m_objChain;
    std::vector<std::unique_ptr<LightSource>> m_lights;
    SDFOperation* m_lastOp{};
    bool m_lastAddedObj{ true };
    std::unique_ptr<SDFBuilder> m_currentBranch{ nullptr };
    std::string m_currentBranchName{};
    template<typename T, typename... Args>
        requires (std::is_base_of_v<SDFObject, T> || std::same_as<SDFObject, T>)
    SDFBuilder& pushSDFObj(Args&&... args);
    size_t loadSerializedBranches(std::vector<std::string>& strVec);
public:
    SDFBuilder();
    std::unique_ptr<SDFObject> getBaseObject();
    std::vector<std::unique_ptr<LightSource>> getLights();
    std::unique_ptr<std::unordered_map <std::string, std::vector<std::string>>> getBranchMap();
    SDFBuilder& addLightSource(Vec3 position = LightSource::DFLT_POSITION, double luminance = LightSource::DFLT_LUMINANCE, Color color = LightSource::DFLT_COLOR, double ambientPower = LightSource::DFLT_AMBIENTPOWER);
    SDFBuilder& stepBack(size_t steps = 1);
    SDFBuilder& color(Color color);
    SDFBuilder& loadSerialized(std::vector<std::string>& strVec, bool addToExisting = false);
    SDFBuilder& startBranch(std::string branchName);
    SDFBuilder& endBranch();
    SDFBuilder& insertBranch(std::string branchName, bool stepBack = true);
    void serialize(std::vector<std::string>& strVec);
    template <std::derived_from<SDFOperation> T, typename... Args>
    SDFBuilder& operation(Args&&... args);
    template <std::derived_from<CombinationOperation> T, std::derived_from<SDFObject> U, typename... ArgsT>
    auto combination(ArgsT&&... argst);
    template <typename T, typename... Args>
        requires (std::is_base_of_v<Animator, T>)
    SDFBuilder& animation(Args&&... args);
};

template<typename T, typename ...Args>
    requires (std::is_base_of_v<SDFObject, T> || std::same_as<SDFObject, T>)
SDFBuilder& SDFBuilder::pushSDFObj(Args && ...args)
{
    if (m_currentBranch) {
        m_currentBranch->pushSDFObj<T>(std::forward<Args>(args)...);
    }
    else {
        auto obj = std::make_unique<T>(std::forward<Args>(args)...);
        m_objChain.push_back(obj.get());
        dynamic_cast<CombinationOperation*>(m_lastOp)->setCombineObject(std::move(obj));
        m_lastAddedObj = true;
    }
    return *this;
}

template <std::derived_from<SDFOperation> T, typename... Args>
SDFBuilder& SDFBuilder::operation(Args&&... args) {
    if (m_currentBranch) {
        m_currentBranch->operation<T>(std::forward<Args>(args)...);
    }
    else {
        m_lastOp = m_objChain.back()->pushOperation<T>(std::forward<Args>(args)...);
        m_lastAddedObj = false;
    }
    return *this;
}

template <std::derived_from<CombinationOperation> T, std::derived_from<SDFObject> U, typename... ArgsT>
auto SDFBuilder::combination(ArgsT&&... argst) {
    return [this, ...captured_args = std::forward<ArgsT>(argst)](auto&&... argsu) mutable -> SDFBuilder& {
        if (m_currentBranch) {
            m_currentBranch->template combination<T, U>(std::forward<ArgsT>(captured_args)...)(std::forward<decltype(argsu)>(argsu)...);
        }
        else {
            auto obj = std::make_unique<U>(std::forward<ArgsT>(captured_args)...);
            auto prev = m_objChain.back();
            m_objChain.push_back(obj.get());
            auto op = std::make_unique<T>(std::move(obj), std::forward<decltype(argsu)>(argsu)...);
            m_lastOp = op.get();
            prev->pushOperation(std::move(op));
            m_lastAddedObj = true;
        }
        return *this;
    };
}

template <typename T, typename... Args>
    requires (std::is_base_of_v<Animator, T>)
SDFBuilder& SDFBuilder::animation(Args&&... args) {
    if (m_currentBranch) {
        m_currentBranch->animation<T>(std::forward<Args>(args)...);
    }
    else if (m_lastAddedObj) {
        m_objChain.back()->bindAnimation(std::move(std::make_unique<T>(std::forward<Args>(args)...)));
    } else if (m_lastOp) {
        m_lastOp->bindAnimation(std::move(std::make_unique<T>(std::forward<Args>(args)...)));
    }
    return *this;
}