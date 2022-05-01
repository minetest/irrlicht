// Copyright (C) 2009-2012 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_E_DRIVER_CHOICE_H_INCLUDED
#define IRR_E_DRIVER_CHOICE_H_INCLUDED

#include <iostream>
#include <cstdio>
#include "EDriverTypes.h"
#include "IrrlichtDevice.h"

namespace irr
{
    
    //! ask user for driver
    static irr::video::E_DRIVER_TYPE driverChoiceConsole(bool allDrivers=false)
    {
#if defined (_IRR_IPHONE_PLATFORM_) || defined (_IRR_ANDROID_PLATFORM_)
        return irr::video::EDT_OGLES2;
#else
        printf("Please select the driver you want:\n");
        irr::u32 i=0;
        char c  = 'a';
        
        for (i=irr::video::EDT_COUNT; i>0; --i)
        {
            if ( allDrivers || irr::IrrlichtDevice::isDriverSupported(irr::video::E_DRIVER_TYPE(i-1)) )
            {
                printf(" (%c) %s\n", c, irr::video::DRIVER_TYPE_NAMES[i-1]);
				++c;
            }
        }

		char userSelection;
        std::cin >> userSelection;
        c = 'a';
        
        for (i=irr::video::EDT_COUNT; i>0; --i)
        {
			if ( allDrivers || irr::IrrlichtDevice::isDriverSupported(irr::video::E_DRIVER_TYPE(i-1)) )
			{
				if (userSelection == c)
					return irr::video::E_DRIVER_TYPE(i-1);
				++c;
			}
        }

        return irr::video::EDT_COUNT;
#endif
    }
    
	/*
		For using an alternative camera in the examples.
		Try to translate the viewpoint (Maya internal CameraRotation)
	*/
	static inline void switchToMayaCamera(IrrlichtDevice* device)
	{
#if 1
		return;
#else
		if (!device) return;

		scene::ICameraSceneNode* camera = device->getSceneManager()->getActiveCamera();
		if (!camera || camera->getID() == 54321) return;


		core::vector3df target = camera->getTarget() - camera->getPosition();
		core::vector3df relativeRotation = target.getHorizontalAngle();

		scene::ICameraSceneNode* maya = device->getSceneManager()->addCameraSceneNodeMaya(
			0, -1500, 1000, 1500,
			54321,
			target.getLength(),
			true,
			relativeRotation.X + 90, relativeRotation.Y
		);
		if (maya)
		{
			maya->setNearValue(camera->getNearValue());
			maya->setFarValue(camera->getFarValue());
		}

		device->getCursorControl()->setVisible(true);
		device->setResizable(true);
#endif
	}

} // end namespace irr

#endif
