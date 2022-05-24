#pragma once

struct RenderStage {

    RenderStage();
    virtual ~RenderStage();

    virtual void render() = 0;

};