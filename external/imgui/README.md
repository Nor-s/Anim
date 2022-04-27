# im-neo-sequencer

## Description
Im-neo-sequencer is my own implementation if animation sequencer for imgui, it was made for my friend who is making his own engine and wasn't happy with [imsequencer](https://github.com/CedricGuillemet/ImGuizmo). Right now it only supports keyframe animations with single frame keyframes, but I'm open for more development in future.  

## Visuals
![im-neo-sequencer](screenshots/screenshot.png "Basic im-nep-sequencer")

## Installation
Its just drag and drop to your project, maybe fix includes but thats all there is to it

## Usage
Using im-neo-sequencer is simple and should follow ImGui API style   
To creae empty sequencer we use Begin and End functions:

```cpp
uint32_t currentFrame = 0;
uint32_t startFrame = 0;
uint32_t endFrame = 64;

if(ImGui::BeginNeoSequencer("Sequencer", &currentFrame, &startFrame, &endFrame)) {
    // Timeline code here
    ImGui::EndNeoSequencer();
}
```

To add timeline, you either use BeginNeoTimeline or if you want collapsable Timeline you can use BeginNeoGroup:

```cpp
if(ImGui::BeginNeoGroup("Transform",&m_pTransformOpen)) {
    std::vector<uint32_t> keys = {0, 10, 24};
    if(ImGui::BeginNeoTimeline("Position", keys )) {
        ImGui::EndNeoTimeLine();
    }
    ImGui::EndNeoTimeLine
}
```
NOTE: There is C interface for BeginNeoTimeline, but I use C++ one for clarity.  
NOTE: I'm planning on making im-neo-sequencer support for C soon, but now I'm using some C++ features in it (context is held in unordered_map)

## Contributing
Feel free to contribute, I'm always open for fixes and improvements

## Authors and acknowledgment
My friend [Simon Gido](https://github.com/SimonGido) who I made this project for and with.

## License
MIT

## Project status
Right now, im-neo-sequencer is in early development, I'm making breaking changes to API and it may be broken, feel free to submit your bugs.
