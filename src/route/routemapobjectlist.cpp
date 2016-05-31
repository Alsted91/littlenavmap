/*****************************************************************************
* Copyright 2015-2016 Alexander Barthel albar965@mailbox.org
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "routemapobjectlist.h"
#include "geo/calculations.h"

const float RouteMapObjectList::INVALID_VALUE = std::numeric_limits<float>::max();

RouteMapObjectList::RouteMapObjectList()
{

}

RouteMapObjectList::~RouteMapObjectList()
{

}

int RouteMapObjectList::getNearestLegIndex(const atools::geo::Pos& pos, float *crossTrackDistance) const
{
  int nearest = -1;
  float minDistance = std::numeric_limits<float>::max();
  float minCrossTrack = std::numeric_limits<float>::max();

  for(int i = 1; i < size(); i++)
  {
    bool valid;
    float crossTrack = pos.distanceMeterToLine(at(i - 1).getPosition(), at(i).getPosition(), valid);
    float distance = std::abs(crossTrack);

    if(valid && distance < minDistance)
    {
      minDistance = distance;
      minCrossTrack = crossTrack;
      nearest = i;
    }
  }
  for(int i = 0; i < size(); i++)
  {
    float distance = at(i).getPosition().distanceMeterTo(pos);
    if(distance < minDistance)
    {
      minDistance = distance;
      minCrossTrack = INVALID_VALUE;
      nearest = i + 1;
    }
  }
  if(crossTrackDistance != nullptr)
  {
    if(minCrossTrack == INVALID_VALUE)
      *crossTrackDistance = INVALID_VALUE;
    else
      *crossTrackDistance = atools::geo::meterToNm(minCrossTrack);

  }
  return nearest;
}

bool RouteMapObjectList::getRouteDistances(const atools::geo::Pos& pos,
                                           float *distFromStartNm, float *distToDestNm,
                                           float *nearestLegDistance, float *crossTrackDistance,
                                           int *nearestLegIndex) const
{
  int index = getNearestLegIndex(pos, crossTrackDistance);
  if(index != -1)
  {
    if(index >= size())
      index = size() - 1;

    if(nearestLegIndex != nullptr)
      *nearestLegIndex = index;

    const atools::geo::Pos& position = at(index).getPosition();
    float distToCur = atools::geo::meterToNm(position.distanceMeterTo(pos));

    if(nearestLegDistance != nullptr)
      *nearestLegDistance = distToCur;

    if(distFromStartNm != nullptr)
    {
      *distFromStartNm = 0.f;
      for(int i = 0; i <= index; i++)
        *distFromStartNm += at(i).getDistanceTo();
      *distFromStartNm -= distToCur;
      *distFromStartNm = std::abs(*distFromStartNm);
    }

    if(distToDestNm != nullptr)
    {
      *distToDestNm = 0.f;
      for(int i = index + 1; i < size(); i++)
        *distToDestNm += at(i).getDistanceTo();
      *distToDestNm += distToCur;
      *distToDestNm = std::abs(*distToDestNm);
    }
    return true;
  }
  return false;
}