#pragma once

#include <bitset>
#include <vector>

const unsigned int MAX_COMPONENTS = 32;

// We use a bitset (1s and 0s) to keep track of which components an entity has,
// and also helps keep track of which entities a system is interested in.
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent {
    protected:
        static int nextId;
};

// Used to assign a unique id to a component type
template <typename T>
class Component: public IComponent {
    // Returns the unique id of Component<T>
    static int GetId() {
        static auto id = nextId++;
        return id;
    }
};

class Entity {
    private:
        int id;

    public:
        Entity(int id): id(id) {};
        Entity(const Entity& entity) = default;

        int GetId() const;

        Entity& operator =(const Entity& other) = default;

        bool operator ==(const Entity& other) const {
            return id == other.GetId();
        }

        bool operator !=(const Entity& other) const {
            return id != other.GetId();
        }

        bool operator >(const Entity& other) const {
            return id > other.GetId();
        }

        bool operator <(const Entity& other) const {
            return id < other.GetId();
        }        
};


// The system processes entities that contains a specific signature
class System {
    private:
        // Entity must have component signature that are equal to component signature
        // of that a particular system requires, for that system to consider that entity.
        Signature componentSignature;
        std::vector<Entity> entities;

    public:
        System() = default;
        ~System() = default;
        
        void AddEntityToSystem(Entity entity);
        void RemoveEntityFromSystem(Entity entity);
        std::vector<Entity> GetSystemEntities() const;
        const Signature& GetComponentSignature() const;

        // Defines the component type that entities must have to be considered by the system 
        template <typename TComponent> void RequireComponent();

};

class Registry {

};


template <typename TComponent> 
void System::RequireComponent() {
    const auto componentId = Component<TComponent>::getId();
    componentSignature.set(componentId);
}