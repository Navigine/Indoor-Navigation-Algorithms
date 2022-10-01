<h1 align="center">
  <img src="resources/logo.jpg"
  alt="Markdown Monster icon"/><br/>
</h1>

Indoor-navigation-algorithms  
[![Build](https://github.com/Navigine/Indoor-Navigation-Algorithms/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/Navigine/Indoor-Navigation-Algorithms/actions/workflows/ubuntu.yml) [![Build](https://github.com/Navigine/Indoor-Navigation-Algorithms/actions/workflows/macos.yml/badge.svg)](https://github.com/Navigine/Indoor-Navigation-Algorithms/actions/workflows/macos.yml)
============================

Navigine team supports the Open Source initiative by offering part of the algorithms for building navigation and indoor tracking. We believe that our code could bring significant benefits to third-party software developers. We regularly test new R&D activities to create extremely precise navigation systems.
Navigine’s mission in relation to Open Source is to act as experts in the field of navigation algorithms, helping the industry of indoor positioning to grow. 

Precise indoor positioning algorithms are the key to implementation of the best location-based services and applications, with the main focus on indoor navigation developed by Navigine company. 

Here we will step by step publish the source code of our algorithm starting with trilateration.

## Useful Links

- Refer to the [Navigine official website](https://navigine.com/) for complete list of downloads, useful materials, information about the company, and so on.
- [Get started](http://client.navigine.com/login) with Navigine to get full access to Navigation services, SDKs, and applications.
- Refer to the Navigine [User Manual](http://docs.navigine.com/) for complete product usage guidelines.
- Find company contact information at the official website under [Contacts](https://navigine.com/contacts/) tab.

## Contents

This repository consists of the following components

* [src](https://github.com/Navigine/Indoor-Positioning-And-Navigation-Algorithms/tree/master/src) - the source code of Navigine positioning algorithms

* [standalone_algorithms](https://github.com/Navigine/Indoor-Positioning-And-Navigation-Algorithms/tree/master/standalone_algorithms) - set of algorithms used in navigation

* [tests](https://github.com/Navigine/Indoor-Positioning-And-Navigation-Algorithms/tree/master/tests) - test and utilities for the evaluation of source code quality

* [tools](https://github.com/Navigine/Indoor-Positioning-And-Navigation-Algorithms/tree/master/tools/verification) - utilities for data extraction from map and settings

## Creating Navigation Client

Here are some examples to give you an idea how to use [Navigation Client](https://github.com/Navigine/Indoor-Positioning-And-Navigation-Algorithms/blob/master/include/navigine/navigation-core/navigation_client.h).

First, create variables that will store your data.
```c++
    // variables for storing location elements
    navigine::navigation_core::GeoLevel geoLevel;
    std::shared_ptr<navigine::navigation_core::LevelCollector> levelCollector;
    navigine::navigation_core::NavigationSettings navigationSettings;
    std::shared_ptr<navigine::navigation_core::NavigationClient> navClient;
```

Add transmitters (Beacon, Eddystone, WIFI, WIFI-RTT, etc.) as `GeoTransmitters` from your location.

Then they will be converted to `XYTransmitters` for internal evaluations
    
    Parameters:
        - id - identifier of transmitter like (major,minor,uuid) for beacon, (namespaceId,instanceId) for eddystone, mac for WIFI;
        - point - latitude and longitude as GeoPoint;
        - pathlossModel - A, B and power of transmitter as `PathlossModel` struct variable;
        - type - type of transmitter, like Beacon, Eddystone, WIFI, WIFI-RTT, etc..

Inside for loop add all the transmitters from your location. Here is an example of adding one element

```c++
    geoLevel.transmitters.emplace_back(transmitterId,
                              navigine::navigation_core::GeoPoint(latitude, longitude),
                              navigine::navigation_core::PathlossModel{A, B, power},
                              navigine::navigation_core::TransmitterType::BEACON);

```

Create geometry of the level using method `getGeometry` from [barriers_geometry_builder.h](https://github.com/Navigine/Indoor-Positioning-And-Navigation-Algorithms/blob/master/include/navigine/navigation-core/barriers_geometry_builder.h) file. Geometry could be created using the barriers and the size of the level
    
    Parameters:
        - barrierList - list of polygons, where each polygon describes the barrier of the level;
        - allowedArea - polygon, which is created using width and height of the map;

Create the list of Polygons which will describe all the barriers

```c++
    std::list<navigine::navigation_core::Polygon> barriersList;
    for (size_t i = 0; i < barriers.size(); ++i)
    {
        auto coords = barriers.at(i);
        navigine::navigation_core::Polygon barrier;
        for (const auto& coord : coords)
            boost::geometry::append(barrier, navigine::navigation_core::Point(coord.first, coord.second));

        barriersList.push_back(barrier);
    }
```

Create the polygon of allowed area

```c++
    navigine::navigation_core::Polygon levelArea;
    auto boundingBox = navigation_core::Box(
                                   navigation_core::Point(leftMin.latitude, leftMin.longitude),
                                   navigation_core::Point(rightMax.latitude, rightMax.longitude));
    boost::geometry::convert(boundingBox, allowedArea);
    geoLevel.geometry = navigine::navigation_core::getGeometry(barriersList, levelArea);
```

Create `LevelCollector` using method `createLevelCollector` and add all available geo levels.

```c++
    levelCollector = navigine::navigation_core::createLevelCollector();
    levelCollector->addGeoLevel(geoLevel);
```

Create `NavigationSettings`, with two parameters - level settings and common settings. (You can find them in navigation.xml i.e.)

Create `NavigationClient` using method `createNavigationClient` which will take as arguments level collector and navigation settings.

```c++
    navClient = navigine::navigation_core::createNavigationClient(levelCollector, navigationSettings);
```

## Navigation test

The example of navigation is presented in a file [Navigation Test](https://github.com/Navigine/Indoor-Positioning-And-Navigation-Algorithms/blob/master/tests/navigation_test.cpp).
The test application takes 3 parameters:
- path to the file of the location in` geojson` format
- path to the file of logs gathered in this location in `json` format
- path to the settings file in `json` format in which parameters of the algorithm are specified

### Build

In order to build the test application go to the root directory of the repository and execute the following commands

``` sh
cmake -H. -Bbuild
cmake --build build
```

To run the tests

``` sh
cd build/
./navigation_test location.geojson log.json settings.json
```

## Evaluate the navigation

Quality of navigation can be evaluated via calculation of different navigational errors.
The most important among them are presented in the following table

| Error         | Description |
| :---          |    :----    |
| `nav_err`     | Average navigation error in meters                                                                    |
| `q_75_err`    | 75% quantile of all the measurements have navigation error less than the value of `nav_75_err`        |
| `avr_fi`      | Average angle error in degrees                                                                        |
| `level_err`   | Percentage of wrongly determined levels                                                               |
| `exec_time`   | Execution time in seconds                                                                             |

## Navigation Algorithms explanation

Here are lectures on navigational algorithms used in this repository
- [Navigine's demo of WIFI-RTT](https://www.youtube.com/watch?v=VAzMhld3bH0&t)
- [Pedometer Algorithm Explained](https://www.youtube.com/watch?v=cpwrwPTqMac&t)
- [Complementary Filter](https://www.youtube.com/watch?v=49mJcFc2xXw)
- [Particle Filter](https://www.youtube.com/watch?v=JFZ-HARdZA8)