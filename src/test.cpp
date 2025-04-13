#include <vector>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_sdlrenderer3.h>

#include <stdio.h>
#include <SDL3/SDL.h>

#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>

#include <nanosvg/nanosvg.h>
#include <nanosvg/nanosvgrast.h>
#include <iostream>
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

extern "C" {
    extern gvplugin_library_t gvplugin_dot_layout_LTX_library;
    extern gvplugin_library_t gvplugin_core_LTX_library; //gvrender_core_svg.c
    lt_symlist_t lt_preloaded_symbols[] = {
	{ "gvplugin_dot_layout_LTX_library", &gvplugin_dot_layout_LTX_library },
    { "gvplugin_core_LTX_library",&gvplugin_core_LTX_library }, 
    { NULL, NULL }
    };
}

void renderGraph(std::vector<uint8_t> &imgDat, int &w, int &h);


void renderGraph(std::vector<uint8_t> &imgData, int &w, int &h) {
    
    const char *dot = "digraph G { A -> B; B -> C; C -> A; }";
    // Create Graphviz context
    //GVC_t *gvc = gvContext();
    extern lt_symlist_t lt_preloaded_symbols[];
    GVC_t *gvc = gvContextPlugins(lt_preloaded_symbols, 0);

    //gvContextPlugins
    // Read graph from DOT string
    Agraph_t *graph = agmemread(dot);
    if (!graph) {

    }

    // Layout graph
    gvLayout(gvc, graph, "dot");

    // Render to SVG in memory
    char *svg_output = nullptr;
    unsigned int length;
    gvRenderData(gvc, graph, "svg", &svg_output, &length);
    std::cout << svg_output << std::endl;

    // svg_output now contains SVG data in memory, and is null-terminated

    char nanosvg_data[length];
    memcpy(nanosvg_data, svg_output, length);
    NSVGimage* img = nsvgParse(nanosvg_data, "px", 96.0f * 20.0f);
   
    w = img->width;
    h = img->height;
    std::cout << "SVG size: " << w << "x" << h << std::endl;
    NSVGrasterizer* rast = nsvgCreateRasterizer();
    imgData.resize(w * h * 4); // RGBA format


    nsvgRasterize(rast, img, 0, 0, 1.0f, imgData.data(), w, h, w * 4);
    nsvgDeleteRasterizer(rast);

    // Clean up
    gvFreeRenderData(svg_output);
    gvFreeLayout(gvc, graph);
    agclose(graph);
    gvFreeContext(gvc);
    nsvgDelete(img);
    
}


// Main code
int main(int, char**)
{
    // Setup SDL
    // [If using SDL_MAIN_USE_CALLBACKS: all code below until the main loop starts would likely be your SDL_AppInit() function]
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return -1;
    }

    // Create window with SDL_Renderer graphics context
    Uint32 window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL3+SDL_Renderer example", 1280, 720, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderVSync(renderer, 1);
    if (renderer == nullptr)
    {
        SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    std::cout << "Window DPI:" << SDL_GetWindowPixelDensity(window) << std::endl;

    std::vector <uint8_t> imgData;
    int graphW, graphH;
    renderGraph(imgData, graphW, graphH);
    std::cout << "Graph size: " << graphW << "x" << graphH << std::endl;

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, graphW, graphH);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_UpdateTexture(texture, nullptr, imgData.data(), graphW * 4);

    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);


    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!done)
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        // [If using SDL_MAIN_USE_CALLBACKS: call ImGui_ImplSDL3_ProcessEvent() from your SDL_AppEvent() function]
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).

   
        {
            ImGuiIO& io = ImGui::GetIO();
            ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
            ImGui::SetNextWindowSize(io.DisplaySize);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::Begin("Hello, world!",  NULL, ImGuiWindowFlags_NoDecoration);                           // Create a window called "Hello, world!" and append into it.
            ImGui::PopStyleVar();
           
            ImGui::BeginChild("1", ImVec2(io.DisplaySize.x * 0.5f, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX);
            char buffer [1024] = "";
            ImGui::InputTextMultiline("##Input",buffer,sizeof(buffer),ImVec2(-1.0f, -1.0f));
            ImGui::EndChild();

            ImGui::SameLine();


            ImGui::BeginChild("2", ImVec2(-1.0f, -1.0f), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeX);

            //   IMGUI_API void          Image(ImTextureID user_texture_id, const ImVec2& image_size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1));
            ImGui::Image((ImTextureID)(intptr_t)texture, ImVec2(graphW, graphH));
            ImGui::EndChild();
            

            ImGui::End();

        }
      

        // Rendering
        ImGui::Render();
        //SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColorFloat(renderer, clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


 /*
            ImGui::BeginTable("Table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable);
            ImGui::TableSetupColumn("Column 1", ImGuiTableColumnFlags_WidthStretch, -1.0f);
            ImGui::TableSetupColumn("Column 2", ImGuiTableColumnFlags_WidthStretch, -1.0f);
            ImGui::TableHeadersRow();
            ImGui::TableNextRow(); // Move to the next row, explicitly making space for 2 columns

            ImGui::TableSetColumnIndex(0);
           // ImGui::BeginChild("Table Container", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);

            //ImGui::EndChild();
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("Column 2");

            ImGui::EndTable();
            //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            
            ImGui::End();
            */