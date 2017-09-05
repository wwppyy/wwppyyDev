// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

///////////////////////////////////////////////////////////
// librealsense tutorial #2 - Accessing multiple streams //
///////////////////////////////////////////////////////////

// First include the librealsense C++ header file
#include <librealsense/rs2.hpp>
#include <cstdio>

// Also include GLFW to allow for graphical display
#include <GLFW/glfw3.h>
#include <librealsense/rsutil2.hpp>

using namespace rs2;
using namespace std;
unsigned long* convertD2RGBA(const uint16_t* temp);
unsigned long* convertIR2RGBA(const char *temp);
unsigned long temp1[640 * 480];
int main()
{
    // Create a context object. This object owns the handles to all connected realsense devices.
    context ctx;
	char testbmp[640 * 480]; //wwppyy use to convert depth format to char to mix with IR
    // Create device_hub to handle device disconnections
    util::device_hub hub(ctx);

    auto finished = false;
    GLFWwindow* win;
    while(!finished)
    {
        try
        {
            // Wait for any device to connect and return it
            auto dev = hub.wait_for_device();
            auto connected_devices = ctx.query_devices();
            printf("There are %u connected RealSense devices.\n", connected_devices.size());


            printf("\nUsing device 0, an %s\n", dev.get_camera_info(RS2_CAMERA_INFO_DEVICE_NAME));
            printf("    Serial number: %s\n", dev.get_camera_info(RS2_CAMERA_INFO_DEVICE_SERIAL_NUMBER));
            printf("    Firmware version: %s\n", dev.get_camera_info(RS2_CAMERA_INFO_CAMERA_FIRMWARE_VERSION));

            vector<rs2_stream> supported_streams = { RS2_STREAM_DEPTH, RS2_STREAM_INFRARED, RS2_STREAM_INFRARED2, RS2_STREAM_COLOR };

            // Configure the relevant subdevices of the RealSense camera
            dev.open({ { RS2_STREAM_DEPTH, 640, 480, 30, RS2_FORMAT_Z16 },
			           { RS2_STREAM_INFRARED, 640, 480, 30, RS2_FORMAT_Y8 },
                       { RS2_STREAM_INFRARED2, 640, 480, 30, RS2_FORMAT_Y8 } });

            // Create frame queue to pass new frames from the device to our application
            frame_queue queue(RS2_STREAM_COUNT);
            // Create a frame buffer to hold on to most up to date frame until a newer frame is available
            frame frontbuffer[RS2_STREAM_COUNT];

            // Start the physical devices and specify our frame queue as the target
            dev.start(queue);

            // Open a GLFW window to display our output
            glfwInit();

            win = glfwCreateWindow(640, 480 * 2, "librealsense tutorial #2",  nullptr, nullptr);
            glfwMakeContextCurrent(win);

            // Determine depth value corresponding to one meter
            auto depth_units = 1.f;
            if (dev.supports(RS2_OPTION_DEPTH_UNITS))
                depth_units = dev.get_option(RS2_OPTION_DEPTH_UNITS);

            // Loop until window closed or device disconnects
            while(!glfwWindowShouldClose(win) && hub.is_connected(dev))
            {
                glfwPollEvents();

                // Move new frames into the frame buffer
                frame frame;
                while (queue.poll_for_frame(&frame))
                {
                    auto stream_type = frame.get_stream_type();
                    // Override the last frame with the new one
                    frontbuffer[stream_type] = move(frame);
                }

                glClear(GL_COLOR_BUFFER_BIT);
                glPixelZoom(1, -1);

                for (auto & stream : supported_streams)
                {
                    if (frontbuffer[stream])
                        switch (stream)
                        {
                        case RS2_STREAM_DEPTH: // Display depth data by linearly mapping depth between 0 and 2 meters to the red channel
							{ 
						glRasterPos2f(-1, 1);
						//glPixelTransferf(GL_RED_SCALE, (0xFFFF * depth_units) / 2.0f);
						//glDrawPixels(640, 480, GL_RED, GL_UNSIGNED_SHORT, frontbuffer[stream].get_data());
						//auto temp1 = (const uint16_t*)frontbuffer[RS2_STREAM_DEPTH].get_data();
						auto temp1 = reinterpret_cast<const uint16_t*>(frontbuffer[RS2_STREAM_DEPTH].get_data());
						auto tempRGBAp = convertD2RGBA(temp1);
						glDrawPixels(640, 480, GL_RGBA, GL_UNSIGNED_BYTE, tempRGBAp);

#if 0
						char *cp = testbmp;
						for (int i = 0; i < 640 * 480; i++)
						{
							*cp = (char)(*temp1 >> 8);
							temp1++;
							cp++;
						}
						//glDrawPixels(640, 480, GL_LUMINANCE, GL_UNSIGNED_BYTE,testbmp);
						glDrawPixels(640, 480, GL_LUMINANCE, GL_UNSIGNED_BYTE, testbmp);
#endif
						break;
							}
#if 0
                        case RS2_STREAM_COLOR: // Display infrared image by mapping IR intensity to visible luminance
							glEnable(GL_BLEND);
							glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                            glRasterPos2f(-1, 0);
                            glDrawPixels(640, 480, GL_LUMINANCE, GL_UNSIGNED_BYTE, frontbuffer[stream].get_data());
                            break;
#endif
                        
#if 1
						case RS2_STREAM_INFRARED2: // Display infrared image by mapping IR intensity to visible luminance
						{
						//glEnable(GL_BLEND);
						//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glRasterPos2f(-1, 0);
						//glPixelTransferf(GL_BLUE_SCALE, 1);
						//glDrawPixels(640, 480, GL_LUMINANCE, GL_UNSIGNED_BYTE, frontbuffer[stream].get_data());
						auto temp2 = reinterpret_cast<const char*>(frontbuffer[RS2_STREAM_INFRARED].get_data());
						auto tempRGBAp2 = convertIR2RGBA(temp2);
						glDrawPixels(640, 480, GL_RGBA, GL_UNSIGNED_BYTE, tempRGBAp2);

						//char *cp = testbmp;
						//blending 
						/* float blend = 0.5;
						//for (int i = 0; i < 640 * 480; i++)
						 // {
						//	*cp = (*cp + *temp2) >> 1;
						//	temp2++;
						//	cp++;
						  }
*/
						//glDrawPixels(640, 480, GL_LUMINANCE, GL_UNSIGNED_BYTE, testbmp);
						//drawline  
						glLineWidth(2.5);
						glColor3f(1.0, 0.0, 0.0);
						glBegin(GL_LINE_STRIP);
						glVertex2f(-0.5f, 0.5f);
						glVertex2f(0.5f, 0.5f);
						glVertex2f(0.5f, -0.5f);
						glVertex2f(-0.5f, -0.5f);
						glVertex2f(-0.5f, 0.5f);
						glEnd();
						break;
						}
#endif

                        default: break;    // This demo will display native streams only
                        }
                }

                glfwSwapBuffers(win);
            }

            // If the window was closed then we are finished,
            // otherwise the cammera probably got disconnected and we will wait until it connects again
            if (glfwWindowShouldClose(win))
                finished = true;
        }
        catch(const error & e)
        {
            // Method calls against librealsense objects may throw exceptions of type error
            printf("error was thrown when calling %s(%s):\n", e.get_failed_function().c_str(), e.get_failed_args().c_str());
            printf("    %s\n", e.what());
        }
        glfwDestroyWindow(win);
        glfwTerminate();
    }
    printf("Finishing up");

    return EXIT_SUCCESS;
}

unsigned long* convertD2RGBA(const uint16_t* temp)
{
	for (int n = 0; n < 640 * 480; n++)
	{
		//fill depth to R
		 temp1[n] = (*temp) >> 2;
		 temp1[n] = temp1[n] << 8; //G
		 //temp1[n] = temp1[n] | 0x00000000;
		//temp1[n] = 0x0000FF00; //0xAABBGGRR
		temp++;
	}
	return temp1;

}

unsigned long* convertIR2RGBA(const char *temp)
{
	for (int n = 0; n < 640 * 480; n++)
	{
		//fill depth to R
		temp1[n] = (*temp) << 2; //R
		//temp1[n] = temp1[n] << 16;
		//temp1[n] = temp1[n] | 0x00000055;
		temp++;
	}
	return temp1;
}