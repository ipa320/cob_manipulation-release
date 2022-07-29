/*
 * Copyright 2017 Fraunhofer Institute for Manufacturing Engineering and Automation (IPA)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <ros/ros.h>
#include <GraspTable.h>

void GraspTable::ReadDoubleValue(TiXmlElement* xml, const char * tag, double * value)
{
  TiXmlHandle handle(xml);
  TiXmlElement* element=handle.FirstChild(tag).Element();
  sscanf(element->GetText(), "%lf", value);
}

void GraspTable::ReadJoint(TiXmlElement* xml, const char * tag, std::vector<double> & values)
{
  TiXmlHandle handle(xml);
  TiXmlElement* element=handle.FirstChild(tag).Element();
  values.resize(16);
  ReadDoubleValue(element, "ThumbAngle",           &(values.data()[0]));
  ReadDoubleValue(element, "ThumbBase",            &(values.data()[1]));
  ReadDoubleValue(element, "ThumbProximal",        &(values.data()[2]));
  ReadDoubleValue(element, "ThumbDistal",          &(values.data()[3]));
  ReadDoubleValue(element, "FirstFingerBase",      &(values.data()[4]));
  ReadDoubleValue(element, "FirstFingerProximal",  &(values.data()[5]));
  ReadDoubleValue(element, "FirstFingerDistal",    &(values.data()[6]));
  ReadDoubleValue(element, "MiddleFingerBase",     &(values.data()[7]));
  ReadDoubleValue(element, "MiddleFingerProximal", &(values.data()[8]));
  ReadDoubleValue(element, "MiddleFingerDistal",   &(values.data()[9]));
  ReadDoubleValue(element, "RingFingerBase",       &(values.data()[10]));
  ReadDoubleValue(element, "RingFingerProximal",   &(values.data()[11]));
  ReadDoubleValue(element, "RingFingerDistal",     &(values.data()[12]));
  ReadDoubleValue(element, "LittleFingerBase",     &(values.data()[13]));
  ReadDoubleValue(element, "LittleFingerProximal", &(values.data()[14]));
  ReadDoubleValue(element, "LittleFingerDistal",   &(values.data()[15]));
}

void GraspTable::ReadPose(TiXmlElement* xml, const char * tag, std::vector<double> & values)
{
  TiXmlHandle handle(xml);
  TiXmlElement* element=handle.FirstChild(tag).Element();
  values.resize(6);
  ReadDoubleValue(element, "PositionX", &(values.data()[0]));
  ReadDoubleValue(element, "PositionY", &(values.data()[1]));
  ReadDoubleValue(element, "PositionZ", &(values.data()[2]));
  ReadDoubleValue(element, "Roll",      &(values.data()[3]));
  ReadDoubleValue(element, "Pitch",     &(values.data()[4]));
  ReadDoubleValue(element, "Yaw",       &(values.data()[5]));
}

int GraspTable::ReadFromFile(const char * filename, GraspTableObject* graspTableObject)
{
  TiXmlDocument doc(filename);
  if (graspTableObject == NULL)
  {
    printf("GraspTable::ReadFromFile:Error,  argument error%s\n",filename);
    return -3;
  }
  if (!doc.LoadFile())
  {
    printf("GraspTable::ReadFromFile:Error, could not read %s\n",filename);
    return -1;
  }
  //printf ("Readig file %s\n",filename);
  TiXmlHandle root_handle(&doc);
  TiXmlHandle grasp_list_handle=root_handle.FirstChild("GraspList");
  TiXmlElement* number_element=grasp_list_handle.FirstChildElement("Number").Element();
  // total number of grasps in this file
  int number;
  sscanf(number_element->GetText(), "%d", &number);

  if (graspTableObject->Init(number) != 0)
  {
    printf("GraspTable::ReadFromFile:Error, could not allocate GraspTableObject\n");
    return -2;
  }
  for (int i=0; i<number; i++)
  {
    TiXmlHandle grasp_handle=grasp_list_handle.ChildElement("Grasp", i);
    TiXmlElement* grasp_element=grasp_handle.Element();
    double quality;
    grasp_element->QueryDoubleAttribute("Quality", &quality);

    Grasp * newGrasp = new Grasp();
    newGrasp->SetGraspId(i);

    std::vector<double> values;

    ReadPose(grasp_element, "ApproachPose", values);
    newGrasp->SetTCPPreGraspPose(values);

    ReadPose(grasp_element, "GraspPose", values);
    newGrasp->SetTCPGraspPose(values);

    ReadJoint(grasp_element, "ApproachJoint", values);
    newGrasp->SetHandPreGraspConfig(values);

    ReadJoint(grasp_element, "GraspJoint", values);
    newGrasp->SetHandGraspConfig(values);

    ReadJoint(grasp_element, "GraspOptimalJoint", values);
    newGrasp->SetHandOptimalGraspConfig(values);

    graspTableObject->AddGrasp(newGrasp);
  }

  return 0;
}


int GraspTable::Init(char* object_table_file,unsigned int table_size)
{
  FILE* f = fopen(object_table_file,"r");
  int numberOfObjects = 0;

  if (f==NULL)
  {
    printf("GraspTable::Error, Object Table File not found :%s\n",object_table_file);
    return -1;
  }
  fscanf(f,"%d\n",&numberOfObjects);
  m_GraspTable.resize(table_size); //range of DESIRE class ids
  for (unsigned int i=0;i < table_size;i++)
  {
    m_GraspTable[i] = NULL;
  }
  for (int obj=0; obj <numberOfObjects; obj++)
  {
    char GraspTableFileName[500];
    int objectClassId = 0;
    fscanf(f,"%d, %s\n",&objectClassId,GraspTableFileName);
//~ #####################################################################################################################
    std::string object_table_file_str=object_table_file;
    unsigned found = object_table_file_str.find_last_of("/");
    std::string filepath = object_table_file_str.substr(0, found);
    std::string grasp_table_file_str = filepath + '/' + GraspTableFileName;
    strncpy(GraspTableFileName, grasp_table_file_str.c_str(), sizeof(GraspTableFileName));
    GraspTableFileName[sizeof(GraspTableFileName) - 1] = 0;
//~ #####################################################################################################################
    printf("GraspTable::Init: Trying to read grasp table for object %d from file %s ...\n",objectClassId,GraspTableFileName);
    GraspTableObject * graspTableObject = new GraspTableObject();
    graspTableObject->SetObjectClassId(objectClassId);
    if (ReadFromFile(GraspTableFileName,graspTableObject)==0)
    {
      printf("successful\n");
      AddGraspTableObject(graspTableObject);
    }
    else
    {
      printf("failed\n");
    }
  }
  return 5;
}

void GraspTable::AddGraspTableObject(GraspTableObject* object)
{
  unsigned int objectClassId = object->GetObjectClassId();
  if (objectClassId < m_GraspTable.size())
  {
    m_GraspTable[objectClassId]=object;
  }
  else
  {
    printf("GraspTable::AddGraspTableObject: Error, class id larger than table size!\n");
  }
}


Grasp* GraspTable::GetNextGrasp(unsigned int objectClassId)
{
  Grasp* retVal = NULL;
  if (objectClassId < m_GraspTable.size() && m_GraspTable[objectClassId] != NULL)
  {
    if ((objectClassId != m_lastObjectClassId))
    {
      m_lastObjectClassId=objectClassId;
      m_GraspTable[objectClassId]->ResetGraspReadPtr();
    }
    retVal =  m_GraspTable[objectClassId]->GetNextGrasp();
  }
  return retVal;
}

void GraspTable::ResetReadPtr(unsigned int object_class_id)
{
  if (object_class_id < m_GraspTable.size())
  {
    m_GraspTable[object_class_id]->ResetGraspReadPtr();
  }
}

Grasp * GraspTable::GetGrasp(unsigned int objectClassId, unsigned int & grasp_id)
{
  Grasp* retVal = NULL;
  if (objectClassId < m_GraspTable.size() && m_GraspTable[objectClassId] != NULL)
  {
    retVal=m_GraspTable[objectClassId]->GetGrasp(grasp_id);
  }
  return retVal;
}
