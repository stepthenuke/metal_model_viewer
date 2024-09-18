/*
 * Copyright 2024 Denis Stepaniuk
 * Licenced under the you jackass license
 */

#include <cstdlib>

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

class ViewDelegate : public MTK::ViewDelegate {
public:
   ViewDelegate(MTL::Device *device) 
      : MTK::ViewDelegate()
      , device_{device->retain()}
      , commandQueue_{device_->newCommandQueue()}
   {}

   ~ViewDelegate() {
      commandQueue_->release();
      device_->release();
   }

   void drawInMTKView(MTK::View *view) override {
      NS::AutoreleasePool *autoRelelasePool = NS::AutoreleasePool::alloc()->init();

      MTL::CommandBuffer *cmd = commandQueue_->commandBuffer();
      MTL::RenderPassDescriptor *rpd = view->currentRenderPassDescriptor();
      MTL::RenderCommandEncoder *enc = cmd->renderCommandEncoder(rpd);
      enc->endEncoding();
      cmd->presentDrawable(view->currentDrawable());
      cmd->commit();

      autoRelelasePool->release();
   }

private:
   /* these shouldn't be here. but who cares? */
   MTL::Device *device_;
   MTL::CommandQueue *commandQueue_;
};

class AppDelegate : public NS::ApplicationDelegate {
public:
   ~AppDelegate() {
      mtkView_->release();
      window_->release();
      device_->release();
      delete viewDelegate_;
   }

   void applicationWillFinishLaunching(NS::Notification *notification) override {
      CGRect frame = CGRectMake(0, 0, 800, 800);
      window_ = NS::Window::alloc()->init(
         frame,
         NS::WindowStyleMaskClosable | NS::WindowStyleMaskTitled,
         NS::BackingStoreBuffered,
         false);
      
      device_ = MTL::CreateSystemDefaultDevice();
      if (device_ == nullptr) std::exit(228); /* who cares? i don't */

      mtkView_ = MTK::View::alloc()->init(frame, device_);
      mtkView_->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
      mtkView_->setClearColor(MTL::ClearColor::Make(1.0, 0.5, 0.6, 1.0)); /* is it pink? */
      
      viewDelegate_ = new ViewDelegate(device_);
      mtkView_->setDelegate(viewDelegate_);

      window_->setContentView(mtkView_);
      /* why strings're so hard? */
      window_->setTitle(NS::String::string("pinky pink", NS::StringEncoding::UTF8StringEncoding));
      window_->makeKeyAndOrderFront(nullptr);

      NS::Application *app = reinterpret_cast<NS::Application *>(notification->object());
      app->activateIgnoringOtherApps(true);
   }

   // void applicationDidFinishLaunching(NS::Notification *nofitification) override {}

   bool applicationShouldTerminateAfterLastWindowClosed(NS::Application *sender) override {
      return true;
   }

private:
   NS::Window *window_;
   MTK::View *mtkView_;
   MTL::Device *device_;
   ViewDelegate *viewDelegate_;
};

int main() {
   NS::AutoreleasePool *autoRelelasePool = NS::AutoreleasePool::alloc()->init();

   AppDelegate delegate;

   NS::Application *sharedApplication = NS::Application::sharedApplication();
   sharedApplication->setDelegate(&delegate);
   sharedApplication->run();

   autoRelelasePool->release(); 
   return 0;
}