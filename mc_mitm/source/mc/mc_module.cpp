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
#include "mc_module.hpp"
#include "mc_service.hpp"

namespace ams::mc {

    namespace {

        enum PortIndex {
            PortIndex_MissionControl,
            PortIndex_Count,
        };

        constexpr sm::ServiceName MissionControlServiceName = sm::ServiceName::Encode("mc");

        struct ServerOptions {
            static constexpr size_t PointerBufferSize   = 0x1000;
            static constexpr size_t MaxDomains          = 0;
            static constexpr size_t MaxDomainObjects    = 0;
            static constexpr bool CanDeferInvokeRequest = false;
            static constexpr bool CanManageMitmServers  = false;
        };

        constexpr size_t MaxSessions = 4;

        class ServerManager final : public sf::hipc::ServerManager<PortIndex_Count, ServerOptions, MaxSessions> {
            private:
                virtual Result OnNeedsToAccept(int port_index, Server *server) override;
        };

        ServerManager g_server_manager;

        sf::UnmanagedServiceObject<IMissionControlInterface, MissionControlService> g_mission_control_service;

        ams::Result ServerManager::OnNeedsToAccept(int port_index, Server *server) {
            switch (port_index) {
                case PortIndex_MissionControl:
                    return this->AcceptImpl(server, g_mission_control_service.GetShared());
                AMS_UNREACHABLE_DEFAULT_CASE();
            }
        }

        constexpr s32 ThreadPriority = 20;
        constexpr size_t ThreadStackSize = 0x1000;
        alignas(os::ThreadStackAlignment) constinit u8 g_thread_stack[ThreadStackSize];
        constinit os::ThreadType g_thread;

        void MissionControlThreadFunction(void *) {
            R_ABORT_UNLESS(g_server_manager.RegisterServer(PortIndex_MissionControl, MissionControlServiceName, MaxSessions));
            g_server_manager.LoopProcess();
        }

    }

    void Launch() {
        R_ABORT_UNLESS(os::CreateThread(&g_thread,
            MissionControlThreadFunction,
            nullptr,
            g_thread_stack,
            ThreadStackSize,
            ThreadPriority
        ));

        os::SetThreadNamePointer(&g_thread, "mc::MissionControlThread");
        os::StartThread(&g_thread);
    }

    void WaitFinished() {
        os::WaitThread(&g_thread);
    }

}
