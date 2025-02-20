/*!*************************************************************************
****
\file LevelEditor.cpp
\author Cheung Jun Yin Matthew
\par DP email: j.cheung@digipen.edu
\par Course: csd2400
\par Section: a
\par Milestone 1
\date 28-9-2022
\brief  This program utilises Dear ImGui and OpenGL to create a editor interface
        to allow us to edit object, create entities and modify the
        properties of the components and save them accordingly.
****************************************************************************
***/

#include "empch.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "imgui/ImGuizmo.h"
#include "LevelEditor.h"
#include "ExoEngine/Timer/Time.h"
#include "ExoEngine/Timer/Fps.h"
#include "Platform/Graphics/Renderer.h"
#include "glm/gtc/type_ptr.hpp"
namespace EM {

    bool color_picker = false;
    bool drop_menu = false;
    int color_picker_slider = 12;
    int drop_menu_slider = 12;
    bool alphabar = true;

    std::unique_ptr<LevelEditor> LevelEditor::m_instance{ nullptr };

    std::unique_ptr<LevelEditor>& LevelEditor::GetInstance()
    {
        if (m_instance == nullptr)
        {
            m_instance = std::make_unique<LevelEditor>();
        }
        return m_instance;
    }

    // Init for levelEditor sets context for ImGui 
    void LevelEditor::Init(Window* window)
    {
        m_window = window;
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows


        ImGui_ImplGlfw_InitForOpenGL(window->GetWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 450");
    }

    //  Update loop for level editor, poll events and set new frames
    void LevelEditor::Update()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuiIO& io = ImGui::GetIO();
        // tells imgui how big our display is 
        io.DisplaySize = ImVec2(static_cast<float>(m_window->Getter().m_Width), static_cast<float>(m_window->Getter().m_Height));
  
        ImGui::ShowDemoWindow();
        docking();
        Profiler();
       
       // DropDownMenu();
        ColorPickerTab();

    }
    //  Render interface onto frame
    void LevelEditor::Draw()
    {
        ImGui::Render();
        ImGui::EndFrame();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* m_window = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(m_window);
        }

    }

    //  End instance
    void LevelEditor::End()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    //  This dropdown menu holds check boxes, sliders dropdown menus etc. This was used to learn how to implement
    //  Widgets, boxes and other useful tools in ImGui, which will be developed in the future 
    void LevelEditor::DropDownMenu()
    {
        
        ImGui::SetNextWindowSize(ImVec2(500, 500)); // ,NULL, ImguiWindowFlags_NoResize
        if (ImGui::Begin("Drop down menu")) //main box for color picker window
        {
            EditorTabs();
            ImGui::Checkbox("Experimental buttons", &drop_menu);
            if (ImGui::Button("Click here"))
            {
                drop_menu = true;
            }
            ImGui::SliderInt("Brightness", &drop_menu_slider, 1, 25);

            if (ImGui::TreeNode("Drag and Drop"))
            {
                if (ImGui::TreeNode("Drag and drop in standard widgets"))
                {
                    // ColorEdit widgets automatically act as drag source and drag target.
                    // They are using standardized payload strings IMGUI_PAYLOAD_TYPE_COLOR_3F and IMGUI_PAYLOAD_TYPE_COLOR_4F
                    // to allow your own widgets to use colors in their drag and drop interaction.
                    // Also see 'Demo->Widgets->Color/Picker Widgets->Palette' demo.
                    static float col1[3] = { 1.0f, 0.0f, 0.2f };
                    static float col2[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
                    ImGui::ColorEdit3("color 1", col1);
                    ImGui::ColorEdit4("color 2", col2);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Drag and drop to copy/swap items"))
                {
                    enum Mode
                    {
                        Mode_Copy,
                        Mode_Move,
                        Mode_Swap
                    };
                    static int mode = 0;
                    if (ImGui::RadioButton("Copy", mode == Mode_Copy)) { mode = Mode_Copy; } ImGui::SameLine();
                    if (ImGui::RadioButton("Move", mode == Mode_Move)) { mode = Mode_Move; } ImGui::SameLine();
                    if (ImGui::RadioButton("Swap", mode == Mode_Swap)) { mode = Mode_Swap; }
                    static const char* names[9] =
                    {
                        "Player", "Enemy", "Weapon",
                        "Powerup", "Health", "Skill",
                        "Skill", "Itemdrop", "drops"
                    };
                    for (int n = 0; n < IM_ARRAYSIZE(names); n++)
                    {
                        ImGui::PushID(n);
                        if ((n % 3) != 0)
                            ImGui::SameLine();
                        ImGui::Button(names[n], ImVec2(60, 60));

                        // Our buttons are both drag sources and drag targets here!
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                        {
                            // Set payload to carry the index of our item (could be anything)
                            ImGui::SetDragDropPayload("DND_DEMO_CELL", &n, sizeof(int));

                            // Display preview (could be anything, e.g. when dragging an image we could decide to display
                            // the filename and a small preview of the image, etc.)
                            if (mode == Mode_Copy) { ImGui::Text("Copy %s", names[n]); }
                            if (mode == Mode_Move) { ImGui::Text("Move %s", names[n]); }
                            if (mode == Mode_Swap) { ImGui::Text("Swap %s", names[n]); }
                            ImGui::EndDragDropSource();
                        }
                        if (ImGui::BeginDragDropTarget())
                        {
                            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
                            {
                                IM_ASSERT(payload->DataSize == sizeof(int));
                                int payload_n = *(const int*)payload->Data;
                                if (mode == Mode_Copy)
                                {
                                    names[n] = names[payload_n];
                                }
                                if (mode == Mode_Move)
                                {
                                    names[n] = names[payload_n];
                                    names[payload_n] = "";
                                }
                                if (mode == Mode_Swap)
                                {
                                    const char* tmp = names[n];
                                    names[n] = names[payload_n];
                                    names[payload_n] = tmp;
                                }
                            }
                            ImGui::EndDragDropTarget();
                        }
                        ImGui::PopID();
                    }
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
        }
        ImGui::End();
    }

    // Color selection tool, yet to be connected to the game engine to be able to edit colors
    void LevelEditor::ColorPickerTab()
    {

       
              
       /* ImGui::Begin("color");
        auto& sprite = p_ecs.GetComponent<Sprite>(1);
        ImGui::Text("Color widget HSV with Alpha:");
        ImGui::End();*/
             
                
            
 /*         static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);

          
           static bool alpha_preview = true;
            static bool alpha_half_preview = false;
            static bool drag_and_drop = true;
            static bool options_menu = true;
            static bool hdr = false;
            ImGui::Checkbox("With Alpha Preview", &alpha_preview);
            ImGui::Checkbox("With Half Alpha Preview", &alpha_half_preview);
            ImGui::Checkbox("With Drag and Drop", &drag_and_drop);
            ImGui::Checkbox("With Options Menu", &options_menu); ImGui::SameLine();
            ImGui::Checkbox("With HDR", &hdr); ImGui::SameLine();
            ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);


            ImGui::Text("Color widget:");
            ImGui::SameLine();
            ImGui::ColorEdit3("MyColor##1", (float*)&color, misc_flags);

            ImGui::Text("Color widget with Float Display:");
            ImGui::ColorEdit4("MyColor##2f", (float*)&color, ImGuiColorEditFlags_Float | misc_flags);

            ImGui::Text("Color button with Picker:");
            ImGui::SameLine();
            ImGui::ColorEdit4("MyColor##3", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | misc_flags);

            ImGui::Text("Color button with Custom Picker Popup:");

            // Generate a default palette. The palette will persist and can be edited.
            static bool saved_palette_init = true;
            static ImVec4 saved_palette[32] = {};
            if (saved_palette_init)
            {
                for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
                {
                    ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
                        saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
                    saved_palette[n].w = 1.0f; // Alpha
                }
                saved_palette_init = false;
            }

            static ImVec4 backup_color;
            bool open_popup = ImGui::ColorButton("MyColor##3b", color, misc_flags);
            ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
            open_popup |= ImGui::Button("Palette");
            if (open_popup)
            {
                ImGui::OpenPopup("mypicker");
                backup_color = color;
            }
            if (ImGui::BeginPopup("mypicker"))
            {
                ImGui::Text("CUSTOM COLOR PICKER");
                ImGui::Separator();
                ImGui::ColorPicker4("##picker", (float*)&color, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
                ImGui::SameLine();

                ImGui::BeginGroup();
                ImGui::Text("Current");
                ImGui::ColorButton("##current", color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
                ImGui::Text("Previous");
                if (ImGui::ColorButton("##previous", backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
                    color = backup_color;
                ImGui::Separator();
                ImGui::Text("Palette");
                for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
                {
                    ImGui::PushID(n);
                    if ((n % 8) != 0)
                        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

                    ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
                    if (ImGui::ColorButton("##palette", saved_palette[n], palette_button_flags, ImVec2(20, 20)))
                        color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!

                    // Allow user to drop colors into each palette entry. Note that ColorButton() is already a
                    // drag source by default, unless specifying the ImGuiColorEditFlags_NoDragDrop flag.
                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
                            memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
                            memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
                        ImGui::EndDragDropTarget();
                    }

                    ImGui::PopID();
                }
                ImGui::EndGroup();

            }

         ImGui::Text("Color button only:");
        static bool no_border = false;
        ImGui::Checkbox("ImGuiColorEditFlags_NoBorder", &no_border);
        ImGui::ColorButton("MyColor##3c", *(ImVec4*)&color, misc_flags | (no_border ? ImGuiColorEditFlags_NoBorder : 0), ImVec2(80, 80));

            ImGui::Text("Color picker:");
            static bool alpha = true;
            static bool alpha_bar = true;
            static bool side_preview = true;
            static bool ref_color = false;
            static ImVec4 ref_color_v(1.0f, 0.0f, 1.0f, 0.5f);
            static int display_mode = 0;
            static int picker_mode = 0;
            //ImGui::Checkbox("With Alpha", &alpha);
            //ImGui::Checkbox("With Alpha Bar", &alpha_bar);
            //ImGui::Checkbox("With Side Preview", &side_preview);
            if (side_preview)
            {
                ImGui::SameLine();
                ImGui::Checkbox("With Ref Color", &ref_color);
                if (ref_color)
                {
                    ImGui::SameLine();
                    ImGui::ColorEdit4("##RefColor", &ref_color_v.x, ImGuiColorEditFlags_NoInputs | misc_flags);
                }
            }
            ImGui::Combo("Display Mode", &display_mode, "Auto/Current\0None\0RGB Only\0HSV Only\0Hex Only\0");
            ImGui::SameLine();
            ImGui::SameLine();
            ImGuiColorEditFlags flags = misc_flags;
            if (!alpha)            flags |= ImGuiColorEditFlags_NoAlpha;        // This is by default if you call ColorPicker3() instead of ColorPicker4()
            if (alpha_bar)         flags |= ImGuiColorEditFlags_AlphaBar;
            if (!side_preview)     flags |= ImGuiColorEditFlags_NoSidePreview;
            if (picker_mode == 1)  flags |= ImGuiColorEditFlags_PickerHueBar;
            if (picker_mode == 2)  flags |= ImGuiColorEditFlags_PickerHueWheel;
            if (display_mode == 1) flags |= ImGuiColorEditFlags_NoInputs;       // Disable all RGB/HSV/Hex displays
            if (display_mode == 2) flags |= ImGuiColorEditFlags_DisplayRGB;     // Override display mode
            if (display_mode == 3) flags |= ImGuiColorEditFlags_DisplayHSV;
            if (display_mode == 4) flags |= ImGuiColorEditFlags_DisplayHex;
            ImGui::ColorPicker4("MyColor##4", (float*)&color, flags, ref_color ? &ref_color_v.x : NULL);

            ImGui::Text("Set defaults in code:");
            ImGui::SameLine();
            if (ImGui::Button("Default: Uint8 + HSV + Hue Bar"))
                ImGui::SetColorEditOptions(ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_PickerHueBar);
            if (ImGui::Button("Default: Float + HDR + Hue Wheel"))
                ImGui::SetColorEditOptions(ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_PickerHueWheel);

            // Always both a small version of both types of pickers (to make it more visible in the demo to people who are skimming quickly through it)
            ImGui::Text("Both types:");
            float w = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.y) * 0.40f;
            ImGui::SetNextItemWidth(w);
            ImGui::ColorPicker3("##MyColor##5", (float*)&color, ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(w);
            ImGui::ColorPicker3("##MyColor##6", (float*)&color, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);

            // HSV encoded support (to avoid RGB<>HSV round trips and singularities when S==0 or V==0)
            static ImVec4 color_hsv(0.23f, 1.0f, 1.0f, 1.0f); // Stored as HSV!
            ImGui::Spacing();
            ImGui::Text("HSV encoded colors");
            ImGui::SameLine();
            ImGui::Text("Color widget with InputHSV:");
            ImGui::ColorEdit4("HSV shown as RGB##1", (float*)&color_hsv, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_Float);
            ImGui::ColorEdit4("HSV shown as HSV##1", (float*)&color_hsv, ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_Float);
            ImGui::DragFloat4("Raw HSV values", (float*)&color_hsv, 0.01f, 0.0f, 1.0f);
*/

       
    }

    void LevelEditor::EditorTabs()
    {
        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("Inspector"))
            {
                ImGui::Text("inspector window");
                TransformTab();
                SpriteRenderer();

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Hierarchy"))
            {
                ImGui::Text("Hierarchy window");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Console"))
            {
                ImGui::Text("Console window");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::Separator();
    
    }

    void LevelEditor::TransformTab()
    {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
        {
            //will have to add a disable key in game when pressing these buttons

            ImGui::PushItemWidth(100.0f);
            
            ImGui::Text("X"); ImGui::SameLine();
            static float posx = 0.10f;
            ImGui::PushID(1);
            ImGui::InputFloat("Y", &posx); ImGui::SameLine();
            ImGui::PopID();

            static float posy = 0.20f;
            ImGui::PushID(2);
            ImGui::InputFloat("Z", &posy); ImGui::SameLine();
            ImGui::PopID();

            static float posz = 0.40f;
            ImGui::InputFloat("Position", &posz); 


            ImGui::Text("X"); ImGui::SameLine();
            static float rotx = 0.10f;
            ImGui::PushID(3);
            ImGui::InputFloat("Y", &rotx); ImGui::SameLine();
            ImGui::PopID();

            static float roty = 0.20f;
            ImGui::PushID(4);
            ImGui::InputFloat("Z", &roty); ImGui::SameLine();
            ImGui::PopID();

            static float rotz = 0.40f;
            ImGui::InputFloat("Rotation", &rotz);

            ImGui::Text("X"); ImGui::SameLine();
            static float scax = 0.10f;
            ImGui::InputFloat("Y", &scax); ImGui::SameLine();

            static float scay = 0.20f;
            ImGui::InputFloat("Z", &scay); ImGui::SameLine();

            static float scaz = 0.40f;
            ImGui::InputFloat("Scale", &scaz); 

            ImGui::PopItemWidth();
        }
    }

    void LevelEditor::SpriteRenderer()
    {
        if (ImGui::CollapsingHeader("Sprite Renderer", ImGuiTreeNodeFlags_None))
        {

            ImGui::Text("Sprite");
            ImGui::Text("Color"); ImGui::SameLine(); 

            ImGui::Text("Color button with Picker:");
            ImGui::SameLine(); 

           
            ImGui::Text("Flip");
            ImGui::Text("Draw Mode");
            ImGui::Text("Mask Interaction");
            ImGui::Text("Sprite Sort Point");

            if (ImGui::CollapsingHeader("Additional Settings", ImGuiTreeNodeFlags_None))
            {
                ImGui::Text("Sorting Layer");
                ImGui::Text("Ordering Layer");
            }
        }
    }

    void LevelEditor::SceneHierarchyWindow()
    {
        ImGui::SetNextWindowSize(ImVec2(500, 500));
        if(ImGui::Begin("Hierarchy"))
        {

        }
    }
    
    //Docking function is based off of ImGui demo's ShowExampleAppDockSpace function which will allow tools to dock with the sides of the
    //level editor. However, as of now it is not yet functional as I have yet to fix the frame layering to allow this function to work
    void LevelEditor::docking()
    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoBackground;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));


        ImGui::Begin("DockSpace", 0, window_flags);



        ImGui::PopStyleVar(3);

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

        ImGui::End();
    }

    void LevelEditor::Profiler()
    {
        ImGui::Begin("Profiler");
        //Opengl information
        ImGui::Text("Opengl Information ");
        ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
        ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
        ImGui::Text("Version: %s", glGetString(GL_VERSION));
        ImGui::Text("\n");

       // how much time per frame and Fps
       ImGui::Text("Application average % .3f ms / frame(% .3f FPS)", 1000.0f / FramePerSec::GetInstance().GetFps(), FramePerSec::GetInstance().GetFps());
       
       //Renderering Information
       ImGui::Text("\n");
       auto Infos = Renderer::GetInfo();
       ImGui::Text("Renderer Information");
       ImGui::Text("Draw Calls: %d", Infos.n_DrawCalls);
       ImGui::Text("Quads: %d", Infos.n_Quad);
       ImGui::Text("Vertices: %d", Infos.TotalVertexUsed());
       ImGui::Text("Indices: %d", Infos.TotalIndexUsed());

        //Todo show the each system consume how much at runtime
        ImGui::End();
    }
}

