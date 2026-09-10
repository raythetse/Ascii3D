#pragma once
#include "SDFBuilder.h"
#include "SDFObject.h"

SDFBuilder::SDFBuilder(Vec3 position, Vec3 rotation, Color color, bool lit) {
    m_baseObject = std::make_unique<SDFObject>(position, rotation, color, lit);
    m_objChain.push_back(m_baseObject.get());
}

std::unique_ptr<SDFObject> SDFBuilder::get() {
    return std::move(m_baseObject);
}

SDFBuilder& SDFBuilder::stepBack(size_t steps)
{
    if (!steps) steps = m_objChain.size() - 1;
    for (size_t i{}; i < steps; ++i)
    {
        if (m_objChain.size() == 1) return *this;
        m_objChain.pop_back();
    }
    return *this;
}

SDFBuilder& SDFBuilder::color(Vec3 color)
{
    m_objChain.back()->setColor(color);
    return *this;
}
