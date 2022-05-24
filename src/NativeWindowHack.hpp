#pragma once

/**
 * Hack for making bgfx not stall on init()
 * @URL https://github.com/bkaradzic/bgfx/issues/2036
 * @param cocoaWindow cocoa window handle recieved from glfwGetCocoaWindow(window). Type is NSWindow
 * @return CAMetalLayer* which has been added to the cocoaWindow
 */
void* addMetalLayerToCocoaWindow(void* cocoaWindow);