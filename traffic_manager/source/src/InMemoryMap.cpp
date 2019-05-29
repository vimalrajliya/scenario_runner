// Implementation for an in memory descrete map representation

#include "InMemoryMap.hpp"
#include<typeinfo>

namespace traffic_manager {

InMemoryMap::InMemoryMap(traffic_manager::TopologyList topology) {
    this->topology = topology;
    this->dense_topology = std::vector<SimpleWaypoint>();
    this->entry_node_map = NodeMap();
    this->exit_node_map = NodeMap();
}
InMemoryMap::~InMemoryMap(){}

std::pair<int, int> InMemoryMap::make_node_key(carla::SharedPtr<carla::client::Waypoint> waypooint) {
    auto current_location = waypooint->GetTransform().location;
    int rounded_x = static_cast<int>(std::round(current_location.x * 10));
    int rounded_y = static_cast<int>(std::round(current_location.y * 10));
    return std::pair<int, int>(rounded_x, rounded_y);
}

void InMemoryMap::setUp(int sampling_resolution){
    for(auto const &pair : this->topology) {
        std::cout << "inMemoryMap::setUp" << std::endl;

        auto begin_waypoint = pair.first;
        auto end_waypoint = pair.second;
        auto current_waypoint = begin_waypoint;

        auto begin_node_key = this->make_node_key(begin_waypoint);
        auto end_node_key = this->make_node_key(end_waypoint);

        typedef std::pair<std::pair<int, int>, SimpleWaypoint*> NodeEntry;
        std::cout << "35" << std::endl;
        this->dense_topology.push_back(SimpleWaypoint(begin_waypoint));
        if(this->entry_node_map.find(begin_node_key) == this->entry_node_map.end())
            this->entry_node_map.insert(NodeEntry(begin_node_key, &(this->dense_topology.back())));
        if(this->exit_node_map.find(begin_node_key) == this->exit_node_map.end());
        else {
            auto exit_node_ptr = this->exit_node_map[begin_node_key];
            exit_node_ptr->setNextWaypoint({&(this->dense_topology.back())});
        }
        std::cout << "44" << std::endl;
        while (
            current_waypoint->GetTransform().location.Distance(
                end_waypoint->GetTransform().location) > sampling_resolution) {
            std::cout << "48" << std::endl;
            current_waypoint = current_waypoint->GetNext(1.0)[0];
            std::cout << "50" << std::endl;
            SimpleWaypoint& previous_wp = this->dense_topology.back();
            this->dense_topology.push_back(SimpleWaypoint(current_waypoint));
            auto topology_ptr = &(this->dense_topology.back());
            topology_ptr->getXYZ();
            std::cout << "53" << std::endl;
            previous_wp.setNextWaypoint({&(this->dense_topology.back())});
            std::cout << "55" << std::endl;
        }
        std::cout << "57" << std::endl;
        this->dense_topology.push_back(SimpleWaypoint(end_waypoint));
        if(this->exit_node_map.find(end_node_key) == this->exit_node_map.end())
            this->exit_node_map.insert(NodeEntry(end_node_key, &(this->dense_topology.back())));
        if(this->entry_node_map.find(end_node_key) == this->entry_node_map.end());
        else {
            this->dense_topology.back().setNextWaypoint({this->entry_node_map[end_node_key]});
        }
        std::cout << "65" << std::endl;
    }
}

SimpleWaypoint* InMemoryMap::getWaypoint(carla::geom::Location location) {
    /* Dumb first draft implementation. Need more efficient code for the functionality */
    SimpleWaypoint* closest_waypoint;
    float min_distance = std::numeric_limits<float>::max();
    for(auto &simple_waypoint : this->dense_topology){
        std::cout<<typeid(simple_waypoint).name()<<std::endl;
        float current_distance = simple_waypoint.distance(location);
        if (current_distance < min_distance) {
            min_distance = current_distance;
            closest_waypoint = &simple_waypoint;
        }
    }

    return closest_waypoint;
}

}
