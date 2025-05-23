/*
 * Copyright (c) 2020-2025 ndeadly
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "atgames_controller.hpp"
#include <stratosphere.hpp>

namespace ams::controller {

    namespace {

        constexpr float StickScaleFactor = float(UINT12_MAX) / UINT8_MAX;

    }

    void AtGamesController::ProcessInputData(const bluetooth::HidReport *report) {
        auto atgames_report = reinterpret_cast<const AtGamesReportData *>(&report->data);

        switch(atgames_report->id) {
            case 0x01:
                this->MapInputReport0x01(atgames_report); break;
            default:
                break;
        }
    }

    void AtGamesController::MapInputReport0x01(const AtGamesReportData *src) {
        if (!m_arcadepanel) {
            // Checking if any of the optional Arcade Control Panel buttons are pressed and and switching the mapping
            if (src->input0x01.a_button || src->input0x01.b_button || src->input0x01.c_button || src->input0x01.x_button || src->input0x01.y_button || src->input0x01.z_button) {
                m_arcadepanel = true;
            }
            m_left_stick.SetData(
                SwitchAnalogStick::Center + 0x7ff * (src->input0x01.nudge_left - src->input0x01.nudge_right),
                SwitchAnalogStick::Center
            );
            m_right_stick.SetData(
                SwitchAnalogStick::Center,
                static_cast<u16>(StickScaleFactor * (UINT8_MAX - src->input0x01.right_stick.x)) & UINT12_MAX
            );
            
            m_buttons.dpad_down  = (src->input0x01.dpad == AtGamesDPad_S)  ||
                                   (src->input0x01.dpad == AtGamesDPad_SE) ||
                                   (src->input0x01.dpad == AtGamesDPad_SW);
            m_buttons.dpad_up    = (src->input0x01.dpad == AtGamesDPad_N)  ||
                                   (src->input0x01.dpad == AtGamesDPad_NE) ||
                                   (src->input0x01.dpad == AtGamesDPad_NW);
            m_buttons.dpad_right = (src->input0x01.dpad == AtGamesDPad_E)  ||
                                   (src->input0x01.dpad == AtGamesDPad_NE) ||
                                   (src->input0x01.dpad == AtGamesDPad_SE);
            m_buttons.dpad_left  = (src->input0x01.dpad == AtGamesDPad_W)  ||
                                   (src->input0x01.dpad == AtGamesDPad_NW) ||
                                   (src->input0x01.dpad == AtGamesDPad_SW);

            m_buttons.A = src->input0x01.play;
            m_buttons.B = src->input0x01.rewind;
            m_buttons.Y = src->input0x01.nudge_front;

            m_buttons.R  = src->input0x01.flipper_right;
            m_buttons.ZR = src->input0x01.flipper_right;
            m_buttons.L  = src->input0x01.flipper_left;
            m_buttons.ZL = src->input0x01.flipper_left; 

            m_buttons.plus  = src->input0x01.home_twirl;
        } else {
            m_left_stick.SetData(
                SwitchAnalogStick::Center + 0x7ff * (src->input0x01.nudge_left - src->input0x01.nudge_right),
                SwitchAnalogStick::Center + 0x7ff * (src->input0x01.nudge_front)
            );
            m_right_stick.SetData(
                SwitchAnalogStick::Center,
                static_cast<u16>(StickScaleFactor * (UINT8_MAX - src->input0x01.right_stick.x)) & UINT12_MAX
            );
            
            m_buttons.dpad_down  = (src->input0x01.dpad == AtGamesDPad_S)  ||
                                   (src->input0x01.dpad == AtGamesDPad_SE) ||
                                   (src->input0x01.dpad == AtGamesDPad_SW);
            m_buttons.dpad_up    = (src->input0x01.dpad == AtGamesDPad_N)  ||
                                   (src->input0x01.dpad == AtGamesDPad_NE) ||
                                   (src->input0x01.dpad == AtGamesDPad_NW);
            m_buttons.dpad_right = (src->input0x01.dpad == AtGamesDPad_E)  ||
                                   (src->input0x01.dpad == AtGamesDPad_NE) ||
                                   (src->input0x01.dpad == AtGamesDPad_SE);
            m_buttons.dpad_left  = (src->input0x01.dpad == AtGamesDPad_W)  ||
                                   (src->input0x01.dpad == AtGamesDPad_NW) ||
                                   (src->input0x01.dpad == AtGamesDPad_SW);

            m_buttons.A = src->input0x01.a_button;
            m_buttons.B = src->input0x01.b_button;
            m_buttons.X = src->input0x01.x_button;
            m_buttons.Y = src->input0x01.y_button;

            m_buttons.R  = src->input0x01.c_button;
            m_buttons.ZR = src->input0x01.flipper_right;
            m_buttons.L  = src->input0x01.z_button;
            m_buttons.ZL = src->input0x01.flipper_left; 

            m_buttons.minus = src->input0x01.rewind;
            m_buttons.plus  = src->input0x01.play;

            m_buttons.home = src->input0x01.home_twirl;
        }
    }

}
