/*
 *  gdrive.cp
 *  gdrive
 *
 *  Created by 郭 家寶 on 12/5/19.
 *  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "gdrive.h"
#include "gdrivePriv.h"

void gdrive::HelloWorld(const char * s)
{
	 gdrivePriv *theObj = new gdrivePriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void gdrivePriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

