#ifndef LOC_H
#define LOC_H


/*
 * LOCATION
 */
// longitude_scale - returns the scaler to compensate for shrinking longitude as you move north or south from the equator
// Note: this does not include the scaling to convert longitude/latitude points to meters or centimeters
float        longitude_scale(const struct Location &loc);

// return distance in meters between two locations
float        get_distance(const struct Location &loc1, const struct Location &loc2);

// return distance in centimeters between two locations
uint32_t     get_distance_cm(const struct Location &loc1, const struct Location &loc2);

// return bearing in centi-degrees between two locations
int32_t      get_bearing_cd(const struct Location &loc1, const struct Location &loc2);

// see if location is past a line perpendicular to
// the line between point1 and point2. If point1 is
// our previous waypoint and point2 is our target waypoint
// then this function returns true if we have flown past
// the target waypoint
bool        location_passed_point(const struct Location & location,
                                  const struct Location & point1,
                                  const struct Location & point2);

/*
  return the proportion we are along the path from point1 to
  point2. This will be less than >1 if we have passed point2
 */
float       location_path_proportion(const struct Location &location,
                               const struct Location &point1,
                               const struct Location &point2);

//  extrapolate latitude/longitude given bearing and distance
void        location_update(struct Location &loc, float bearing, float distance);

// extrapolate latitude/longitude given distances north and east
void        location_offset(struct Location &loc, float ofs_north, float ofs_east);

/*
  return the distance in meters in North/East plane as a N/E vector
  from loc1 to loc2
 */
Vector2f    location_diff(const struct Location &loc1, const struct Location &loc2);

/*
 * check if lat and lng match. Ignore altitude and options
 */
bool        locations_are_same(const struct Location &loc1, const struct Location &loc2);

/*
  print a int32_t lat/long in decimal degrees
 */
void        print_latlon(AP_HAL::BetterStream *s, int32_t lat_or_lon);

// Converts from WGS84 geodetic coordinates (lat, lon, height)
// into WGS84 Earth Centered, Earth Fixed (ECEF) coordinates
// (X, Y, Z)
void        wgsllh2ecef(const Vector3d &llh, Vector3d &ecef);

// Converts from WGS84 Earth Centered, Earth Fixed (ECEF)
// coordinates (X, Y, Z), into WHS84 geodetic
// coordinates (lat, lon, height)
void        wgsecef2llh(const Vector3d &ecef, Vector3d &llh);

#endif