/*
 *  This is the platform checker header file.
 *  The platform checker checks what operating system the code is
 *  being compiled on and creates the following preprocessor
 *  definitions:
 *    - DP_BUILD_WINDOWS - For Windows OS's
 *    - DP_BUILD_MAC     - For MacOS's
 *    - DP_BUILD_LINUX   - For Linux and some UNIX based OS's
 *  Of course, this file will be included when ever we need to know
 *  which platform specific libraries are needed.
 *
 *  Created by Joseph Bingheim "The Wizard".
 *
 *  File created: 11/6/2020
 *  Last modified: 11/18/2020
 *
 *  See bottom of file to see updates.
 */

#ifndef _DIRECT_PIXELS_PLATFORM_TEST_H_
#define _DIRECT_PIXELS_PLATFORM_TEST_H_

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#define DP_BUILD_WINDOWS
#elif defined(__APPLE__)
#define DP_BUILD_MAC
#elif defined(__linux__) || defined(__LINUX__) || defined(__unix__) || defined(__UNIX__)
#define DP_BUILD_LINUX
#endif

#endif /* End file */

/*  Notes 11/7/2020 (Joseph):
 *      Simple process, nothing much to note. Added Windows, Mac, and Linux
 *      checks but will be working on one platform at a time. First with windows
 *      and then linux. I have no idea if I will ever get a round to actually adding
 *      the code to support Mac, because as your stereotypical programmer, I really hate
 *      Mac.
 *      Will probably not add to many notes from here because of how straight forward the file is.
 *  Notes 11/18/2020 (Joseph):
 *      Added more common checks to determine which operating system the program is being compiles
 *      on. Also, I used the wrong apple preprocessor. Changed the linux branch from #else to
 *      #elif defined(...) || ...
 */
