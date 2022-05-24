
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <Cocoa/Cocoa.h>

    void* addMetalLayerToCocoaWindow(void* cocoaWindow) {
        NSWindow* window = (NSWindow*)cocoaWindow;
        NSView* contentView = [window contentView];
        CAMetalLayer *res = [CAMetalLayer layer];
        [contentView setWantsLayer:YES];
        [contentView setLayer:res];
        return res;
    }
