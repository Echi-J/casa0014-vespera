
# Experiment Log

CASA0014 — Vespera Interactive Controller

This document records the experiments, observations, challenges and lessons learned during the development of the prototype.

## 26/10/2026 — Initial Hardware Testing

### Completed
- Tested HW-763 capacitive touch sensors with an Arduino MKR WiFi 1010.
- Successfully detected `TOUCHED` and `NOT TOUCHED` states.
- Two HW-763 sensor modules were successfully tested separately. Testing both sensors together on the same Arduino is planned for the next iteration.

### Observations
- Encountered and investigated a faulty sensor module.
- Found that the orientation of soldered header pins affects the physical touch interface.

### Current Limitations
- Touch sensor inputs have not yet been integrated with MQTT.
- The final interaction concept is still under development.

### Next Steps
- Document wiring and hardware configurations.
- Explore interaction concepts and create sketches.
- Integrate touch inputs with MQTT-based lighting control.

### Troubleshooting 01 — Faulty Touch Sensor

**Problem**

During the initial test, the HW-763 capacitive touch sensor failed to respond to touch. The Arduino did not detect the expected touch signal.

**Troubleshooting Process**

1. **Checking the power supply:** I initially suspected a voltage issue. I used a multimeter to measure the supply voltage and confirmed that it was correct.
2. **Inspecting the hardware:** After ruling out an incorrect supply voltage, I examined the sensor module more closely and noticed that a component appeared to be missing.
3. **Identifying the suspected fault:** The missing component appeared to be the TTP223 capacitive touch sensing IC, suggesting that the module was physically defective.
4. **Replacing the sensor:** I replaced the faulty module with another HW-763 sensor and soldered a header pin for testing.
5. **Testing the replacement:** The new sensor successfully detected touch, producing the expected `TOUCHED` and `NOT TOUCHED` messages in the Arduino Serial Monitor.

**Understanding the TTP223**

The TTP223 is a capacitive touch sensing integrated circuit (IC). Its main functions are:

- **Capacitance detection:** Detecting small changes in capacitance when a finger touches or approaches the sensing area.
- **Touch recognition:** Determining whether the detected change represents a valid touch event.
- **Digital output:** Sending a digital signal to the Arduino through its output pin. In the commonly used active-high configuration, the output is LOW when untouched and HIGH when touched.

**Lessons Learned**

- A correct supply voltage does not necessarily mean that a sensor module is functioning properly.
- Visual inspection of hardware components can be just as important as checking the wiring and code.
- Replacing a suspected faulty component and repeating the same test is a useful way to isolate a hardware problem.
- Although the replacement test supports the hardware fault hypothesis, the exact cause of the original module's failure was not independently confirmed.

### Troubleshooting 02 — Connector Compatibility

**Problem**

I considered using a blue touch sensor module at beginning. However, I could not find a compatible connector cable.

**Observation**

The blue sensor was not tested, as I could not establish a suitable connection to the Arduino.

**Decision**

I continued prototyping with the red HW-763 modules, which could be connected using the available components.

**Lesson Learned**

Hardware compatibility and the availability of suitable connectors can influence component selection, even before electronic testing begins.

### Troubleshooting 03 — Header Pin Orientation and Usability

**Problem**

When soldering the header pins onto the red touch sensor module, I positioned them on the touch-sensitive side of the board.

**Observation**

Although the sensor worked electronically, the protruding pins made the touch-sensitive area difficult to access comfortably.

**Design Implications**

This highlighted the importance of considering physical interaction during hardware assembly, rather than focusing only on electrical functionality.

**Next Iteration**

- Reconsider the orientation of the header pins when assembling additional modules.
- Explore ways to make the touch-sensitive surface more accessible.
- Consider how the sensor could be incorporated into a more intuitive physical interface.

**Lesson Learned**

A technically functional prototype is not necessarily a usable interface. Physical assembly decisions can directly affect the user experience.


---

## 28 October 2026 — Touch Ring and MQTT Integration

### Experiment 04 — Six-Sensor Touch Ring

**Objective**

To develop a circular touch interface using six HW-763 capacitive touch sensors.

**Progress**
- Connected six touch sensors to an Arduino MKR WiFi 1010.
- Tested the touch inputs.
- Developed a method to identify finger movement around the circular arrangement.
- Explored using touch direction to control light colour.

**Result**

The touch ring successfully detected user interactions and provided input for controlling colour changes.

### Experiment 05 — MQTT Integration

**Objective**

To connect the physical touch interface to the Vespera lighting installation using MQTT.

**Progress**
- Integrated touch sensor input with MQTT communication.
- Tested colour control through the Lumi interface.
- Successfully controlled the physical Vespera lights.

**Result**

The touch interface was able to control the lighting installation through MQTT.

### Experiment 06 — Colour Transition Refinement

**Problem**

Although the initial colour changes worked, transitions between the six colours appeared too abrupt when moving quickly around the touch ring.

**Iteration 1 — Smoother Transitions**

Modified the colour transition behaviour to create a smoother visual experience.

**Iteration 2 — Colour Palette**

The initial colours felt too saturated and visually harsh.

I experimented with a softer, more muted colour palette inspired by pastel and Morandi colours.

However, the lighter colours made the changes less noticeable.

**Iteration 3 — Final Colour Adjustment**

Adjusted the palette again to achieve a better balance between soft colours and visible transitions.

**Result**

The updated colours and transitions were tested on the physical Vespera installation and produced a more satisfactory visual result.

### Reflection

This process highlighted the importance of testing an interactive system in its intended physical environment.

A colour transition that works technically may not necessarily produce a satisfying visual experience.

The project also demonstrated how physical touch input, Arduino processing, MQTT communication and light output can work together as an interactive IoT system.


### Experiment 07 — Battery Power and Physical Assembly

**Objective**

To develop a self-contained physical controller that can operate without a continuous USB connection to a laptop.

**Development**
- Installed a suitable battery power supply.
- Assembled the Arduino, wiring and touch sensors inside a cardboard enclosure.
- Mounted six touch sensors on the interaction surface.
- Organised the internal components to create a portable physical interface.

**Result**

The physical controller has been assembled, and the battery power setup has been successfully implemented.

The prototype combines capacitive touch input, Arduino processing and MQTT communication to control the Vespera lighting installation.

### Current Project Status

**Functional medium-fidelity prototype completed.**

The core interaction has been implemented and tested:

- Six capacitive touch sensors arranged as a circular interface.
- Touch interaction and direction detection.
- Wireless lighting control using MQTT.
- Smooth colour transitions with a refined colour palette.
- Physical enclosure and battery-powered hardware setup.

The prototype now provides a functional basis for further design refinement and evaluation.

### Further Development and Refinement

Although the core prototype is complete, several improvements could enhance its usability and interaction experience:

1. **Physical interface:** Improve the appearance and accessibility of the touch-sensitive areas.
2. **Interaction feedback:** Explore how to make touch responses more intuitive and engaging.
3. **Colour behaviour:** Further refine the relationship between finger movement and lighting transitions.
4. **Enclosure design:** Improve component arrangement, cable management and structural stability.
5. **User evaluation:** Observe how other users interact with the controller and identify potential usability improvements.

These are opportunities for refinement rather than requirements for completing the current functional prototype.


