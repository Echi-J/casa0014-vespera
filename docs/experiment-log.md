
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
