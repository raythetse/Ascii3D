#include "SceneObjectBuilder.h"

#pragma once
#include "SceneObject.h"

SOBuilder::SOBuilder(Vec3 position, Vec3 rotation, Color color, bool lit) {
    m_baseObject = std::make_unique<SceneObject>(position, rotation, color, lit);
    m_objChain.push_back(m_baseObject.get());
}

std::unique_ptr<SceneObject> SOBuilder::get() {
    return std::move(m_baseObject);
}

SOBuilder& SOBuilder::back() {
    if (m_objChain.size() == 1) return *this;
    m_objChain.pop_back();

    return *this;
}

SOBuilder& SOBuilder::back(size_t steps)
{
    for (size_t i{}; i < steps; ++i)
    {
        back();
    }
    return *this;
}

SOBuilder& SOBuilder::backFull() {
    while (m_objChain.size() > 1) {
        back();
    }
    return *this;
}