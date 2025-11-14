# GitHub Copilot Instructions for CarSpeedBoy

## Project Overview
CarSpeedBoy is an IVI (In-Vehicle Infotainment) application for Automotive Grade Linux (AGL) that displays an animated character whose expression changes based on vehicle speed.

## Architecture
- **Data Acquisition Layer**: Communicates with AGL VSS (Vehicle Signal Specification) via AFB WebSocket API
- **Business Logic Layer**: Speed monitoring and expression state machine
- **Presentation Layer**: Qt/QML based UI with character animations

## Technology Stack
- **Language**: C++17
- **Framework**: Qt5 (Core, Gui, Qml, Quick, WebSockets)
- **Build System**: CMake
- **Platform**: Automotive Grade Linux (AGL)
- **Data Format**: JSON (nlohmann/json)

## Coding Guidelines

### C++ Style
- Follow Modern C++ (C++17) best practices
- Use smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- Prefer RAII for resource management
- Use `const` correctness
- Naming conventions:
  - Classes: `PascalCase`
  - Functions/methods: `camelCase`
  - Member variables: `snake_case_` (trailing underscore)
  - Constants: `UPPER_SNAKE_CASE` or `kConstantName`

### Qt Specifics
- Use Qt signals/slots for event handling
- Leverage Qt's meta-object system with Q_OBJECT
- Use Qt containers (`QString`, `QVector`, etc.) for Qt-specific code
- Use STL containers for generic C++ code
- QML files should use declarative style

### Code Organization
- Header files (.h) in `include/` directory
- Implementation files (.cpp) in `src/` directory
- One class per file
- Forward declarations in headers when possible
- Include guards using `#pragma once`

### Error Handling
- Use Qt logging: `qDebug()`, `qInfo()`, `qWarning()`, `qCritical()`
- Return `bool` for success/failure operations
- Use exceptions sparingly (Qt style)
- Validate all external data (JSON from VSS)

### Documentation
- Use Doxygen-style comments for public APIs
- Document all class members and methods
- Include parameter descriptions and return values
- Example:
  ```cpp
  /**
   * @brief Brief description
   * @param param_name Description
   * @return Description of return value
   */
  ```

### VSS Integration
- Never implement CAN/OBD-II protocol handlers in the application
- Always use VSS standard paths (e.g., `Vehicle.Speed`)
- Connect via AFB WebSocket API
- Handle connection loss and reconnection gracefully

### Testing
- Write unit tests for business logic
- Mock external dependencies (VSS, AFB)
- Use Qt Test framework
- Aim for >80% code coverage

## Project-Specific Rules

### Speed Thresholds
The application uses these default speed ranges:
- Relaxed: 0-20 km/h
- Normal: 21-60 km/h
- Alert: 61-100 km/h
- Warning: 101-120 km/h
- Scared: 121+ km/h

### Configuration
- App configuration in `config.json`
- Protocol configuration is managed by AGL platform (not app responsibility)
- User settings should be persisted

### Performance
- Target embedded automotive systems (limited resources)
- Optimize for low latency (<100ms for speed updates)
- Minimize memory allocations
- Use efficient data structures

### Security
- Never hardcode credentials
- Use AFB authentication tokens
- Validate all input data
- Follow automotive security best practices

## Common Patterns

### Singleton Pattern (avoid)
Prefer dependency injection over singletons for testability.

### Observer Pattern
Use Qt signals/slots for event-driven architecture.

### State Machine
Use explicit state enums and clear transition logic.

## When Generating Code

1. **Always include necessary headers**
2. **Follow the existing project structure**
3. **Add TODO comments for incomplete implementations**
4. **Use descriptive variable and function names**
5. **Add error handling and logging**
6. **Consider thread safety (Qt runs in main thread by default)**
7. **Test edge cases (e.g., negative speeds, connection loss)**

## File Templates

### Header File Template
```cpp
#pragma once

#include <QObject>

/**
 * @brief Brief description of the class
 */
class ClassName : public QObject {
    Q_OBJECT

public:
    explicit ClassName(QObject* parent = nullptr);
    ~ClassName();

signals:
    // Qt signals here

private slots:
    // Qt slots here

private:
    // Private members here
};
```

### Implementation File Template
```cpp
#include "class_name.h"
#include <QDebug>

ClassName::ClassName(QObject* parent)
    : QObject(parent)
{
    qInfo() << "ClassName created";
}

ClassName::~ClassName() {
    qInfo() << "ClassName destroyed";
}
```

## Avoid

- Direct CAN/OBD-II protocol handling
- Hardcoded file paths (use configuration)
- Platform-specific code (keep it portable)
- Blocking operations on the main thread
- Raw pointers for ownership
- C-style casts (use `static_cast`, `qobject_cast`)

## Prioritize

- Clean, readable code over clever optimizations
- Testability and maintainability
- Clear error messages for debugging
- Proper resource cleanup
- Documentation for public APIs

## Questions to Ask Before Implementing

1. Is this feature part of the application or platform responsibility?
2. Does this require VSS data or is it app-specific configuration?
3. Can this be unit tested easily?
4. Is error handling comprehensive?
5. Is this thread-safe?
6. Does this follow Qt best practices?

## Remember

This application runs in a safety-critical automotive environment. Always prioritize:
- **Reliability** over features
- **Safety** over convenience
- **Clarity** over brevity
