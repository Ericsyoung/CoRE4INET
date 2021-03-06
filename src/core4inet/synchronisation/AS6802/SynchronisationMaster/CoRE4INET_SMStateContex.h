//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
/*
 * StateContex.h
 *
 *  Created on: Feb 13, 2012
 *      Author: Lazar Todorov
 */

#ifndef SMSTATECONTEX_H_
#define SMSTATECONTEX_H_
#include "CoRE4INET_SMState.h"
#include "CoRE4INET_SM.h"

namespace CoRE4INET{

class SMStateContex{

public:
		SMState *pState;



	SMStateContex(){}
	virtual ~SMStateContex(){
	    delete(pState);
	}

	virtual  void setState(SMState *firstState){
		pState=firstState;
	}

	virtual void parseMessage(cMessage *message){

		pState->handleMessage(message);

	}
};


}

#endif /* SMSTATECONTEX_H_ */
