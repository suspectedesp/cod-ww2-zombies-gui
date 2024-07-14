#pragma once
#ifndef LOOPS_HPP
#define LOOPS_HPP

#include "../includes.h"
#include "offsets.h"

using namespace std;

//being able to change bool outside because of atomic
atomic<bool> changedisplayname(true);

void DisplayUsername(HANDLE handle, uintptr_t modulebase)
{
    uintptr_t user_address = RPM<uintptr_t>(handle, modulebase + self::UsernameAddress) + self::UsernameOffset;

    std::string username = GetValueFromJSON("username", "default_username");
    std::string usernameType = GetValueFromJSON("username-type", "static");

    std::string modifiedUsername;
    
    while (true)
    {
        if (usernameType == "static")
        {
            changedisplayname = false;
            while (!changedisplayname)
            {
                modifiedUsername = username;
                WPM(handle, user_address, modifiedUsername);
                username = GetValueFromJSON("username", "default_username");
                usernameType = GetValueFromJSON("username-type", "static");
                if (usernameType != "static")
                {
                    changedisplayname = true;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
        else if (usernameType == "dynamic") {
            changedisplayname = true;
            while (changedisplayname) {
                for (size_t i = 0; i < username.size(); ++i) 
                {
                    for (int j = 1; j <= 9; ++j) 
                    {
                        modifiedUsername = "^" + std::to_string(j) + username.substr(0, i + 1);
                        WPM(handle, user_address, modifiedUsername);
                        std::this_thread::sleep_for(std::chrono::milliseconds(33));
                    }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(35));

                for (int i = username.size() - 1; i >= 0; --i) 
                {
                    for (int j = 1; j <= 9; ++j) 
                    {
                        modifiedUsername = "^" + std::to_string(j) + username.substr(0, i + 1);
                        WPM(handle, user_address, modifiedUsername);
                        std::this_thread::sleep_for(std::chrono::milliseconds(33));
                    }
                }
                username = GetValueFromJSON("username", "default_username");
                usernameType = GetValueFromJSON("username-type", "static");
                if (usernameType != "dynamic")
                {
                    changedisplayname = false;
                }
            }

            // Set back to default username after exiting loop
            modifiedUsername = username;
            WPM(handle, user_address, modifiedUsername);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        
    }
}
#endif // LOOPS_HPP