// Copyright 2015 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef FPL_FLATUI_H
#define FPL_FLATUI_H

#include <functional>
#include <string>

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#include "font_manager.h"
#include "flatui/version.h"
#include "fplbase/asset_manager.h"
#include "fplbase/input.h"
#include "mathfu/constants.h"

namespace flatui {

/// @file
/// @addtogroup flatui_core
//
/// @{

/// @brief The core function that drives the GUI.
///
/// While FlatUI i sbeing initialized, it will implicitly load the shaders used
/// in the API below via AssetManager (`shaders/color.glslv`,
/// `shaders/color.glslf`, `shaders/font.glslv`, `shaders/font.glslf`,
/// `shaders/textured.glslv`, and `shaders/textured.glslf`).
///
/// @param[in,out] assetman The AssetManager you want to use textures from.
/// @param[in] fontman The FontManager to be used by the GUI.
/// @param[in] input The InputSystem to be used by the GUI.
/// @param[in] gui_definition A function that defines all GUI elements using the
/// GUI element construction functions. (It will be run twice, once for the
/// layout, and once for rendering & events.)
void Run(fplbase::AssetManager &assetman, FontManager &fontman,
         fplbase::InputSystem &input,
         const std::function<void()> &gui_definition);

/// @enum Event
///
/// @brief Event types are returned by most interactive elements. These are
/// flags, because multiple events may occur during one frame, and thus should
/// be tested using a Bitwise operators (`&`, `|`, etc.).
///
/// For example, it is not uncommon for the value to be
/// `kEventWentDown | kEventWentUp` (or `3`), if the click/touch was quicker
/// than the current frametime.
///
/// You can then check if a specific event occured using the Bitwise AND (`&`)
/// operator. For instance, given an `Event my_event`, you could check if
/// the `kEventWentDown` Event happened in that frame with
/// `if((my_event & kEventWentDown) != 0)`.
///
/// **Enumerations**:
///
/// * `kEventNone` (`0`) - No Event occured in the frame. (This is also returned
///                        by all elements during the layout pass.
/// * `kEventWentUp` (`1`) - Pointing device (or button) was released this
///                          frame, while over this element. (This only triggers
///                          if the element was also the one to receive the
///                          corresponding `kEventWentDown`.
/// * `kEventWentDown` (`2`) - Pointing device is currently being held down on
///                            top of this element. You're not guaranteed to
///                            also receive a `kEventWentUp`, as the pointing
///                            device may have moved to another element (or
///                            no element) before the frame ends.
/// * `kEventIsDown` (`4`) - Pointing device is currently being held down on
///                          top of this element. You're not guaranteed to
///                          receive this event between `kEventWentDown` and
///                          a `kEventWentUp`. That occurs only if the event
///                          spans multiple frames. This only triggers for
///                          the element that the corresponding `kEventWentDown`
///                          fired on.
/// * `kEventStartDrag` (`8`) - Pointing device started dragging this frame
///                             while over this element. The element is expected
///                             to call `CapturePointer()` to receive the drag
///                             event continuously, even if the pointer goes off
///                             of the element.
/// * `kEventEndDrag` (`16`) - Pointing device finished dragging in this frame.
/// * `kEventIsDragging` (`32`) - Pointing device is currently in dragging mode.
/// * `kEventHover` (`64`) - Pointing device is currently over the element, but
///                          not pressed. This event does NOT occur on touch
///                          screen devices. It only occurs for devices that use
///                          a mouse (or a gamepad that emulates a mouse when
///                          selecting). As such, it is good to show a subtle
///                          form of highlighting upon this event. However, the
///                          UI should not rely on it to function.
///
/// For example, a typical drag operation would receive the following events
/// in this sequence: `kEventWentDown` --> `kEventIsDown` (until the pointer
/// motion exceeds a threshold) --> `kEventStartDrag` --> `kEventIsDragging`
/// --> `kEventEndDrag`.
///
enum Event {
  kEventNone = 0,
  kEventWentUp = 1,
  kEventWentDown = 2,
  kEventIsDown = 4,
  kEventStartDrag = 8,
  kEventEndDrag = 16,
  kEventIsDragging = 32,
  kEventHover = 64,
};

// clang-format off
/// @enum Alignment
///
/// @brief Alignment of the groups.
///
/// @note: `kAlignTop` and `kAlignLeft`(as well as `kAlignBottom` and
/// `kAlignRight`) are intended to be aliases of one another, as they both
/// express the same thing on their respective axis.
///
/// **Enumerations**:
///
/// * `kAlignTop` or `kAlignLeft` (`1`) - Align along the top (or left,
///                                       depending on the axis).
/// * `kAlignCenter` (`2`) - Align along the center of the axis.
/// * `kAlignBottom` or `kAlignRight` (`3`) - Align along the bottom (or right,
///                                           depending on the axis).
enum Alignment {
  kAlignTop = 1,
  kAlignLeft = 1,
  kAlignCenter = 2,
  kAlignBottom = 3,
  kAlignRight = 3
};

/// @enum Direction
///
/// @brief Direction of the groups.
///
/// **Enumerations**:
///
/// * `kDirHorizontal` (`4`) - The direction of the group is horizontal
///                            (x-axis).
/// * `kDirVertical` (`8`) - The direction of the group is vertical (y-axis).
/// * `kDirOverlay` (`12`) - The group of elements are placed on top of one
///                          another (along the z-axis).
enum Direction {
  kDirHorizontal = 4,
  kDirVertical = 8,
  kDirOverlay = 12
};

/// @enum Layout
///
/// @brief Specify how to layout a group.
///
/// Elements can be positioned either horizontally or vertically. The elements
/// can be aligned on either side, or centered.
///
/// For example, `kLayoutHorizontalTop` indicates that the elements are layed
/// out from left-to-right, with items of uneven height being aligned from the
/// top.
///
/// In this example, we have three elements: `A` with a height of 3, `B` with
/// a height of 1, and `C` with a height of 2. So we lay the elements out from
/// left to right in the order `A`->`B`->`C`, and we align them along the top.
///
/// That layout would look like this:
///
/// A @htmlonly &nbsp @endhtmlonly  B @htmlonly &nbsp @endhtmlonly  C
///
/// A @htmlonly &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp @endhtmlonly C
///
/// A
///
/// **Enumerations**:
///
/// * `kLayoutHorizontalTop` (`5`) - Lay out the elements horizontally, aligning
///                                  elements of uneven height along the top.
/// * `kLayoutHorizontalCenter` (`6`) - Lay out the elements horizontally,
///                                     aligning elements of uneven height along
///                                     the center.
/// * `kLayoutHotizontalBottom` (`7`) - Lay out the elements horizontally,
///                                     aligning elements of uneven height along
///                                     the bottom.
/// * `kLayoutVerticalLeft` (`9`) - Lay out the elements vertically, aligning
///                                 elements of uneven width along the left.
/// * `kLayoutVerticalCenter` (`10`) - Lay out the elements vertically, aligning
///                                   elements of uneven width along the center.
/// * `kLayoutVerticalRight` (`11`) - Lay out the elements vertically, aligning
///                                  the elements of uneven width along the
///                                  right.
/// * `kLayoutOverlay` (`14`) - Lay out the elements on top of one another, from
///                             the center.
enum Layout {
  kLayoutHorizontalTop =    kDirHorizontal| kAlignTop,
  kLayoutHorizontalCenter = kDirHorizontal| kAlignCenter,
  kLayoutHorizontalBottom = kDirHorizontal| kAlignBottom,
  kLayoutVerticalLeft =     kDirVertical  | kAlignLeft,
  kLayoutVerticalCenter =   kDirVertical  | kAlignCenter,
  kLayoutVerticalRight =    kDirVertical  | kAlignRight,
  kLayoutOverlay =          kDirOverlay   | kAlignCenter,
};
// clang-format on

/// @var kDefaultGroupID
///
/// @brief A sentinel value for group IDs.
const char *const kDefaultGroupID = "__group_id__";

/// @struct Margin
///
/// @brief Specifies the margins for a group, in units of virtual
/// resolution.
struct Margin {
  /// @brief Create a Margin with all four sides of equal size.
  ///
  /// @param[in] m A float size to be used as the margin on all sides.
  Margin(float m) : borders(m) {}

  /// @brief Create a Margin with the left and right sizes of `x`, and top
  /// and bottom sizes of `y`.
  ///
  /// @param[in] x A float size to be used as the margin for the left and
  /// right sides.
  /// @param[in] y A float size to be used as the margin for the right and
  /// left sides.
  Margin(float x, float y) : borders(x, y, x, y) {}

  // Create a margin specifying all 4 sides individually.
  /// @brief Creates a margin specifying all four sides individually.
  ///
  /// @param[in] left A float size to be used as the margin for the left side.
  /// @param[in] top A float size to be used as the margin for the top side.
  /// @param[in] right A float size to be used as the margin for the right side.
  /// @param[in] bottom A float size to be used as the margin for the bottom
  /// side.
  Margin(float left, float top, float right, float bottom)
      : borders(left, top, right, bottom) {}

  /// @var borders
  ///
  /// @brief A vector of four floats containing the values for the
  /// four sides of the margin.
  ///
  /// The internal layout of the margin is: `left`, `top`, `right`, `bottom`.
  mathfu::vec4 borders;
};

/// @brief Converts a virtual screen coordinate to a physical value.
///
/// @param[in] v A mathfu::vec2 vector representing a virtual screen coordinate.
///
/// @return Returns a mathfu::vec2i containing the physical value.
mathfu::vec2i VirtualToPhysical(const mathfu::vec2 &v);

/// @brief Converts a physical screen coordinate to a virtual value.
///
/// @param[in] v A mathfu::vec2i vector representing a phsyical screen
/// coordinate.
///
/// @return Returns a mathfu::vec2 containing the virtual value.
mathfu::vec2 PhysicalToVirtual(const mathfu::vec2i &v);

/// @brief Get the scaling factor for the virtual resolution.
///
/// @return Returns a float representing the scaling factor.
float GetScale();

/// @brief Render an image as a GUI element.
///
/// @param[in] texture A Texture corresponding to the image that should be
/// rendered.
/// @param[in] ysize A float containing the vertical size in virtual resolution.
///
/// @note The x-size will be derived automatically based on the image
/// dimensions.
void Image(const fplbase::Texture &texture, float ysize);

/// @brief Render a label as a GUI element.
///
/// @param[in] text A C-string in UTF-8 format to be displayed as the label.
/// @param[in] ysize A float containing the vertical size in virtual resolution.
///
/// @note The x-size will be derived automatically based on the text length.
void Label(const char *text, float ysize);

/// @brief Render a multi-line version of a label as a GUI element.
///
/// @param[in] text A C-string in UTF-8 format to be displayed as the label.
/// @param[in] ysize A float containing the vertical size in virtual resolution.
/// @param[in] size The max size of the label in virtual resolution. A `0` for
/// `size.y` indicates no height restriction. The API renders the whole text in
/// the label in this case.
void Label(const char *text, float ysize, const mathfu::vec2 &size);

/// @brief Set the Label's text color.
///
/// @param[in] color A vec4 representing the RGBA values that the text color
/// should be set to.
void SetTextColor(const mathfu::vec4 &color);

/// @brief Set the Label's font.
///
/// @param[in] font_name A C-string corresponding to the name of the font
/// that should be set.
void SetTextFont(const char *font_name);

/// @brief Set a locale used for the text rendering.
///
/// @param[in] locale A C-string corresponding to the of the
/// language defined in ISO 639 and country code defined in ISO 3166 connected
/// by '-'. (e.g. 'en-US').
/// The API sets language, script and layout direction used for following text
/// renderings.
void SetTextLocale(const char *locale);

/// @brief Override a text layout direction set by SetTextLocale() API.
///
/// @param[in] direction TextLayoutDirection specifying text layout direction.
void SetTextDirection(const TextLayoutDirection direction);

/// @brief Renders an edit text box as a GUI element.
///
/// @param[in] ysize A float containing the vertical size in virtual resolution.
/// @param[in] size A mathfu::vec2 reference to the size of the edit box in
/// virtual resolution. A `0` for `size.x` indicates an auto expanding text box.
/// A `0` for `size.y` indicates a single line label.
/// @param[in] id A C-string in UTF-8 format to uniquely idenitfy this edit box.
/// @param[in] string A pointer to a C-string in UTF-8 format that should
/// be used as the Label for the edit box.
///
/// @return Returns `true` if the widget is in edit.
bool Edit(float ysize, const mathfu::vec2 &size, const char *id,
          std::string *string);

/// @brief Create a group of elements with a given layout and intra-element
/// spacing.
///
/// @note `StartGroup()` and `EndGroup()` calls must be matched. They may,
/// however, be nested to create more complex layouts.
///
/// @param[in] layout The Layout to be used by the group.
/// @param[in] spacing A float corresponding to the intra-element spacing for
/// the group.
/// @param[in] id A C-string in UTF-8 format to uniquely identify this group.
void StartGroup(Layout layout, float spacing = 0,
                const char *id = kDefaultGroupID);

/// @brief Clean up the Group element start by `StartGroup()`.
///
/// @note `StartGroup()` and `EndGroup()` calls must be matched. They may,
/// however, be nested to create more complex layouts.
void EndGroup();

/// @brief Sets the margin for the current group.
///
/// @note This function is specific to a group, and should be called after
/// `StartGroup()` and before any elements.
///
/// @param[in] margin The Margin to set for the group.
void SetMargin(const Margin &margin);

/// @brief Check for events from the current group.
///
/// Calling `CheckEvent()` marks the current element as an "interactive"
/// element. Each interactive elements needs to have a unique ID in order to
/// properly to receive a keyboard/gamepad focus.
///
/// IDs for Labels and Images are derived from hashing its contents. The user
/// can also specify IDs for CustomElement and Edit elements as an argument.
///
/// If multiple interactive elements have the same ID, a keyboard/gamepad focus
/// navigation will not work as expected (e.g. a focus jump to other elements
/// that share the same ID while naviating with a gamepad).
///
/// @note This function is specific to a group, and should be called after
/// `StartGroup()` and before any elements.
///
/// @return Returns the Event type for the group.
Event CheckEvent();

/// @brief Check for events from the current group.
///
/// @param[in] check_dragevent_only A bool to check if only a drag event
/// occurred (ignore button events). If an element is not interested in
/// button events, then this flag should be set by the caller because elements
/// can only receive the WENT_UP event if it is the same element that received
/// the corresponding WENT_DOWN event.
///
/// @note This function is specific to a group, and should be called after
/// `StartGroup()` and before any elements.
///
/// @return Returns the Event type for the group.
Event CheckEvent(bool check_dragevent_only);

/// @brief Set the default keyboard/gamepad focus to the current element.
///
void SetDefaultFocus();

// Call inside of a group that is meant to be like a popup inside of a
// kLayoutOverlay. It will cause all interactive elements in all groups that
// precede it to not respond to input.
/// @brief Called inside of a group that that is mean to act like a popup inside
/// of a `kLayoutOverlay`.
///
/// It will cause all interactive elements in the groups that precede it to not
/// respond to input.
///
/// @note This function is specific to a group, and should be called after
/// `StartGroup()` and before any elements.
void ModalGroup();

/// @brief Caputre a pointer event.
///
/// After an API call, the element with `element_id` will exclusively receive
/// pointer events until `ReleasePointer()` is called. This API is used mainly
/// for a drag operation, when an element wants to receive events continuously.
///
/// @note This function is specific to a group, and should be called after
/// `StartGroup()` and before any elements.
///
/// @param[in] element_id A C-string in UTF-8 format that contains the ID of the
/// element that should capture all pointer events.
void CapturePointer(const char *element_id);

/// @brief Release a pointer capture.
///
/// @note This function is specific to a group, and should be called after
/// `StartGroup()` and before any elements.
void ReleasePointer();

/// @brief Get the index of th ecaptured pointer.
///
/// This should be used in conjunction with `CheckEvent()` to determine whether
/// a drag operation is in progress.
///
/// @note This function is specific to a group, and should be called after
/// `StartGroup()` and before any elements.
///
/// @return Returns the index of the pointer as a `ssize_t`. Otherwise it
/// returns `-1` if no pointer was captured.
ssize_t GetCapturedPointerIndex();

/// @brief Set the scroll speed of the drag, mouse wheel, and gamepad
/// operations.
///
/// The defaults are `kScrollSpeedDragDefault`, `kScrollSpeedWheelDefault`,
/// and `kScrollSpeedGamepadDefault`.
///
/// @param[in] scroll_speed_drag A float determining the new scroll speed
/// for dragging.
/// @param[in] scroll_speed_wheel A float determining the new scroll speed
/// for the mouse scroll wheel.
/// @param[in] scroll_speed_gamepad A float determining the new scroll speed
/// for a gamepad.
///
/// @note This function is specific to a group, and should be called after
/// `StartGroup()` and before any elements.
void SetScrollSpeed(float scroll_speed_drag, float scroll_speed_wheel,
                    float scroll_speed_gamepad);

/// @brief Set a threshold value for the start of a drag operation.
///
/// The default value is `kDragStartThresholdDefault`.
///
/// @param[in] drag_start_threshold An int determining the new threshold
/// value for the start of a drag operation.
///
/// @note This function is specific to a group, and should be called after
/// `StartGroup()` and before any elements.
void SetDragStartThreshold(int drag_start_threshold);

/// @brief Set the background color for the group.
///
/// @param[in] color A vec4 representing the background color that should be
/// set in RGBA.
///
/// @note This function is specific to a group, and should be called after
/// `StartGroup()` and before any elements.
void ColorBackground(const mathfu::vec4 &color);

/// @brief Set the background texture for the group.
///
/// @note This function is specific to a group, and should be called after
/// `StartGroup()` and before any elements.
///
/// @param[in] tex The Texture to be set as the background for the group.
void ImageBackground(const fplbase::Texture &tex);

/// @brief Set the background texture for the group with nine patch settings.
///
/// In the `patch_info`, the user can define nine patch settings as
/// `vec4(x0, y0, x1, y1)`, where `(x0, y0)` corresponds to the top-left
/// corner of the stretchable area in UV coordinates and `(x1, y1)` corresponds
/// to the bottom-right corner of stretchable area in UV coordinates.
///
/// The coordinates are in UV value in the texture (0.0 ~ 1.0).
///
/// For more information for nine patch, refer
/// http://developer.android.com/guide/topics/graphics/2d-graphics.html#nine-patch
///
/// @note This function is specific to a group, and should be called after
/// `StartGroup()` and before any elements.
///
/// @param[in] tex The Texture of the background image that should be rendered.
/// @param[in] patch_info The nine-patch settings for the corners of the
/// stretchable area in UV coordinates.
void ImageBackgroundNinePatch(const fplbase::Texture &tex,
                              const mathfu::vec4 &patch_info);

/// @brief Make the current group into a scrolling group that can display
/// arbitrary sized elements inside a window of "size", scrolled to the current
/// "offset" (which the caller should store somewhere that survives the current
/// frame).
///
/// @param[in] size A vec2 corresponding to the size of the window that the
/// elements should be displayed in.
/// @param[out] offset A vec2 that captures the value of the current scroll
/// location.
///
/// @note Call `StartScroll()` right after `StartGroup()`.
void StartScroll(const mathfu::vec2 &size, mathfu::vec2 *offset);

/// @brief Ends the current scrolling group.
///
/// @note Call `EndScroll()` right before `EndGroup()`.
void EndScroll();

/// @brief Make the current group into a slider group that can handle basic
/// slider behavior. The group will capture/release the pointer as necessary.
///
/// @param[in] direction Sets the horizontal or vertical scroll direction.
/// @param[in] scroll_margin Sets the margin around the scroll bar.
/// @param[out] value Captures the float output of the slider value.
///
/// @note Call `StartSlider()` right after `StartGroup()`.
void StartSlider(Direction direction, float scroll_margin, float *value);

/// @brief Ends the current slider group.
///
/// @note Call `EndSlider()` right before `EndGroup()`.
void EndSlider();

/// @brief Create a custom element with a given size.
///
/// @param[in] virtual_size The size of the element in virtual screen
/// coordinates.
/// @param[in] id A C-string in UTF-8 format corresponding to the unique
/// ID for the CustomElement.
/// @param[in] renderer The function that is invoked during the render pass
/// to render the element.
void CustomElement(
    const mathfu::vec2 &virtual_size, const char *id,
    const std::function<
        void(const mathfu::vec2i &pos, const mathfu::vec2i &size)> renderer);

/// @brief Render a Texture to a specific position with a given size.
///
/// @note This is usually called in `CustomElement()`'s callback function.
///
/// @param[in] tex The Texture to render.
/// @param[in] pos The position that `tex` should be rendered at, in physical
/// screen coordinates.
/// @param[in] size The size that `tex` should be rendered at, in physical
/// screen coordinates.
void RenderTexture(const fplbase::Texture &tex, const mathfu::vec2i &pos,
                   const mathfu::vec2i &size);

/// @brief Render a Texture to a specific position with a given size and color.
///
/// @note This is usually called in `CustomElement()`'s callback function.
///
/// @param[in] tex The Texture to render.
/// @param[in] pos The position that `tex` should be rendered at, in physical
/// screen coordinates.
/// @param[in] size The size that `tex` should be rendered at, in physical
/// screen coordinates.
/// @param[in] color A vec4 representing the color of the texture that should
/// be rendered in RGBA.
void RenderTexture(const fplbase::Texture &tex, const mathfu::vec2i &pos,
                   const mathfu::vec2i &size, const mathfu::vec4 &color);

/// @brief Render a nine-patch texture at a specific position with a given
/// size.
///
/// @note This is usually used in `CustomElement()`'s callback function.
///
/// In the `patch_info`, the user can define nine patch settings as
/// `vec4(x0, y0, x1, y1)`, where `(x0, y0)` corresponds to the top-left
/// corner of the stretchable area in UV coordinates and `(x1, y1)` corresponds
/// to the bottom-right corner of stretchable area in UV coordinates.
///
/// The coordinates are in UV value in the texture (0.0 ~ 1.0).
///
/// For more information for nine patch, refer
/// http://developer.android.com/guide/topics/graphics/2d-graphics.html#nine-patch
///
/// @param[in] tex The Texture to render.
/// @param[in] patch_info The nine-patch settings for the corners of the
/// stretchable area in UV coordinates.
/// @param[in] pos The position that `tex` should be rendered at, in physical
/// screen coordinates.
/// @param[in] size The size that `tex` should be rendered at, in physical
/// screen coordinates.
void RenderTextureNinePatch(const fplbase::Texture &tex,
                            const mathfu::vec4 &patch_info,
                            const mathfu::vec2i &pos,
                            const mathfu::vec2i &size);

/// @var FLATUI_DEFAULT_VIRTUAL_RESOLUTION
///
/// @brief The default virtual resolution, if none is set.
const float FLATUI_DEFAULT_VIRTUAL_RESOLUTION = 1000.0f;

/// @brief Set the virtual resolution of the smallest dimension of the screen
/// (the Y size in landscape mode, or X in portrait).
///
/// All dimensions specified elsewhere (in floats) are relative to this value.
///
/// The default value, if this function is not called, is
/// `FLATUI_DEFAULT_VIRTUAL_RESOLUTION`.
///
/// If you wish to use native pixels, call this with `min(screen_x, screen_y)`.
///
/// @note This should be called as the first thing in your GUI definition.
///
/// @param[in] virtual_resolution A float representing the virtual resolution
/// of the smallest dimension of the screen.
void SetVirtualResolution(float virtual_resolution);

/// @return Returns the virtual resolution of the screen.
mathfu::vec2 GetVirtualResolution();

/// @brief Position a group within the screen as a whole.
///
/// @note This should be called as the first thing in any top level groups
/// (either your root group, or the children of your root, if the root is
/// `kLayoutOverlay`.
///
/// @param[in] horizontal The alignment for the x-axis of the group (defaults
/// to `left`).
/// @param[in] vertical The alignment for the y-axis of the group (defaults to
/// `top`).
/// @param[in] offset A vec2 that allows you to displace elements from the
/// given alignment.
void PositionGroup(Alignment horizontal, Alignment vertical,
                   const mathfu::vec2 &offset);

/// @brief By default, FlatUI sets up a projection matrix for all the rendering
/// that covers the entire screen (as given by `Renderer::window_size()`. You
/// can call to this function instead, to use whatever projection is in place
/// before `Run()` is called (which may be a 2D or 3D projection).
///
/// @param[in] canvas_size Specifies the new canvas size for the UI to live
/// inside of.
void UseExistingProjection(const mathfu::vec2i &canvas_size);

/// @return Returns the position of the current group in virtual coordinates.
///
/// This is the top/left location of the group. When used in conjunction with
/// `GroupSize()`, this can be used t ocalculate the extents of the group.
mathfu::vec2 GroupPosition();

/// @return Returns the current group's size in virtual coordinates. This
/// function is useful to implement UI that requires other element's sizes, such
/// as a scroll bar.
mathfu::vec2 GroupSize();

/// @return Returns `true` if the last click event was a touch screen or mouse
/// event. Otherwise, it returns `false` (e.g. from a gamepad or keyboard).
bool IsLastEventPointerType();
/// @}

// Returns the version of the FlatUI Library.
const FlatUiVersion *GetFlatUiVersion();

}  // namespace flatui

#endif  // FPL_FLATUI_H
