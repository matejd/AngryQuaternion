//----------------------------------------------------------------------------------
// File:        NvAppBase/MainHtml5.cpp
// SDK Version: v1.2 
// Email:       gameworks@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2014, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------------

// This file is based on MainLinux64.cpp, which uses GLFW 3. Since Emscripten
// still doesn't support that version (latest is 2.7), code was backported.
// Backporting also required changing InputCallbacksGLFW.cpp (new file InputCallbacksHtml5.cpp).
//
#ifdef EMSCRIPTEN

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <time.h>

#include <GL/glew.h>
#include <GL/glfw.h>

#include <emscripten/emscripten.h>

#include "NvAppBase/NvAppBase.h"
#include "NV/NvStopWatch.h"
#include "NvAssetLoader/NvAssetLoader.h"
#include "NV/NvLogs.h"

class NvHtml5StopWatch: public NvStopWatch
{
public:
    NvHtml5StopWatch() : start_time(), diff_time(0.0) {};
    ~NvHtml5StopWatch() {};

public:
    void start() {
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        m_running = true;
    }

    void stop() {
        diff_time = getDiffTime();
        m_running = false;
    }

    void reset() {
        diff_time = 0;
        if (m_running)
            start();
    }

    const float getTime() const {
        if (m_running) {
            return getDiffTime();
        } else {
            return diff_time;
        }
    }

private:
    float getDiffTime() const {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        return  (float) ((now.tv_sec - start_time.tv_sec)
                    + (0.000000001 * (now.tv_nsec - start_time.tv_nsec)) );
    }

    struct timespec start_time;
    float diff_time;
};

static NvAppBase *sApp = NULL;
NvInputCallbacks* sCallbacks = NULL;
extern void setInputCallbacksGLFW();
static bool sWindowIsFocused = true;
static bool sHasResized = true;
static int32_t sForcedRenderCount = 0;

class NvGLHtml5AppContext: public NvGLAppContext {
public:
    NvGLHtml5AppContext(NvEGLConfiguration& config) :
        NvGLAppContext(NvGLPlatformInfo(
            NvGLPlatformCategory::PLAT_MOBILE, 
            NvGLPlatformOS::OS_LINUX))
    {
        mConfig = config;
    }

    bool bindContext() {
        return true;
    }

    bool unbindContext() {
        return true;
    }

    bool swap() {
        glfwSwapBuffers();
        return true;
    }

    bool setSwapInterval(int32_t interval) {
        glfwSwapInterval(interval);
        return true;
    }

    int32_t width() {
        int32_t w, h;
        glfwGetWindowSize(&w, &h);
        return w;
    }

    int32_t height() {
        int32_t w, h;
        glfwGetWindowSize(&w, &h);
        return h;
    }

    GLproc getGLProcAddress(const char* procname) {
        return (GLproc)glfwGetProcAddress(procname);
    }

    bool isExtensionSupported(const char* ext) {
        return glfwExtensionSupported(ext) ? true : false;
    }

    void setConfiguration(const NvEGLConfiguration& config) { mConfig = config; }

    virtual void* getCurrentPlatformContext() { 
        return NULL;
    }

    virtual void* getCurrentPlatformDisplay() { 
        return NULL;
    }

protected:
};

class NvHtml5PlatformContext : public NvPlatformContext {
public:
    NvHtml5PlatformContext() {}
    ~NvHtml5PlatformContext() {}

    virtual bool isAppRunning();
    virtual void requestExit() {}
    virtual bool pollEvents(NvInputCallbacks* callbacks);
    virtual bool isContextLost() { return false; }
    virtual bool isContextBound() { return true; }
    virtual bool shouldRender();
    virtual bool hasWindowResized();
    virtual NvGamepad* getGamepad() { return NULL; }
    virtual void setAppTitle(const char* title) { glfwSetWindowTitle(title); }
    virtual const std::vector<std::string>& getCommandLine() { return m_commandLine; }

    std::vector<std::string> m_commandLine;
protected:
};

bool NvHtml5PlatformContext::isAppRunning() {
    return true;
}

bool NvHtml5PlatformContext::pollEvents(NvInputCallbacks* callbacks) {
    sCallbacks = callbacks;
    glfwPollEvents();
    // In the browser, we need to keep sCallbacks non-null.
    //sCallbacks = NULL;
    return true;
}

bool NvHtml5PlatformContext::shouldRender() {
    if (sWindowIsFocused || (sForcedRenderCount > 0)) {
        if (sForcedRenderCount > 0)
            sForcedRenderCount--;

        return true;
    }
    return false;
}

bool NvHtml5PlatformContext::hasWindowResized() {
    if (sHasResized) {
        sHasResized = false;
        return true;
    }
    return false;
}

static void reshape(int32_t width, int32_t height )
{
    glViewport(0, 0, width, height);
    sHasResized = true;
    sForcedRenderCount += 2;
}

static void focus(int32_t focused)
{
    sWindowIsFocused = (focused != 0);
    sApp->focusChanged(sWindowIsFocused);
    sForcedRenderCount += 2;
}

int32_t main(int32_t argc, char *argv[])
{
    int32_t width, height;
    sWindowIsFocused = true;
    sForcedRenderCount = 0;

    NvAssetLoaderInit(NULL);
    if (glfwInit() != GL_TRUE) {
        LOGE("Failed to initialize GLFW!");
        exit(EXIT_FAILURE);
    }

    NvHtml5PlatformContext* platform = new NvHtml5PlatformContext;
    for (int i = 1; i < argc; i++) {
        platform->m_commandLine.push_back(argv[i]);
    }

    sApp = NvAppFactory(platform);
    NvEGLConfiguration config(NvGfxAPIVersionGL4(), 8,8,8,8, 24,0);
    sApp->configurationCallback(config);

    NvGLHtml5AppContext* context = new NvGLHtml5AppContext(config);
    if (glfwOpenWindow(800, 600, 8,8,8,0, 24,0, GLFW_WINDOW) != GL_TRUE) {
        LOGE("Failed to open GLFW window!");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    sApp->setGLContext(context);
    glfwSetWindowSizeCallback(reshape);
    setInputCallbacksGLFW();
    context->bindContext();
    glfwSwapInterval(1);
    glfwGetWindowSize(&width, &height);

    config.apiVer = NvGfxAPIVersionES2();
    glGetIntegerv(GL_RED_BITS, (GLint*)&config.redBits);
    glGetIntegerv(GL_GREEN_BITS, (GLint*)&config.greenBits);
    glGetIntegerv(GL_BLUE_BITS, (GLint*)&config.blueBits);
    glGetIntegerv(GL_ALPHA_BITS, (GLint*)&config.alphaBits);
    glGetIntegerv(GL_DEPTH_BITS, (GLint*)&config.depthBits);
    glGetIntegerv(GL_STENCIL_BITS, (GLint*)&config.stencilBits);
    context->setConfiguration(config);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        LOGE("Error: %s\n", glewGetErrorString(err));
        exit(-1);
    }

    reshape(width, height);
    sApp->mainLoop();
    delete sApp;
    glfwTerminate();
    NvAssetLoaderShutdown();
    exit(EXIT_SUCCESS);
}

NvStopWatch* NvAppBase::createStopWatch() {
    return new NvHtml5StopWatch;
}

bool NvAppBase::showDialog(const char*, const char *, bool exitApp) {
    return false;
}

bool NvAppBase::writeScreenShot(int32_t, int32_t, const uint8_t*, const std::string&) {
    return false;
}

bool NvAppBase::writeLogFile(const std::string&, bool, const char*, ...) {
    return false;
}

void NvAppBase::forceLinkHack() {
}

#endif
