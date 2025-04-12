# Low Pass FIR Filter with Pipeline Implementation

![License](https://img.shields.io/badge/license-MIT-blue.svg)

> Low-pass FIR filter implemented with Xilinx Vivado, including pipeline optimization and energy consumption analysis

## Project Overview

This project designs and implements a 32-coefficient low-pass FIR (Finite Impulse Response) filter, with an attempt to optimize circuit performance and energy consumption through pipelining techniques. The main goal is to explore how to reduce circuit energy consumption to below 3/4 of the original circuit.

## Features

- 32-coefficient low-pass FIR filter design
- Optimal filter design methodology
- Fixed-point simulation and quantization error analysis
- Pipeline implementation for energy consumption optimization
- Detailed circuit performance and energy consumption analysis

## Filter Specifications

- **Filter Type**: Low-pass FIR filter
- **Filter Order**: 32
- **Input Range**: -1 ~ 1
- **Integer Bits**: 3 bits
- **Optimal Fraction Bits**: 18 bits
- **MAC Word Length**: 25 bits

## Design Implementation

### Filter Coefficients

```
0.002925,  0.009433, -0.003661, -0.010883, -0.001388,  0.016707,  0.010008, -0.020391,
-0.024933,  0.018951,  0.047936, -0.006416, -0.085462, -0.035785,  0.185252,  0.403305,
0.403305,  0.185252, -0.035785, -0.085462, -0.006416,  0.047936,  0.018951, -0.024933,
-0.020391,  0.010008,  0.016707, -0.001388, -0.010883, -0.003661,  0.009433,  0.002925
```

### Frequency Response

This FIR filter design exhibits typical low-pass characteristics, effectively filtering high-frequency signals. The frequency response plot is available in the `docs/images` directory.

### Fixed-Point Simulation

Detailed analysis for fixed-point implementation:
- Input word length: 21 bits (3 bits integer + 18 bits fractional)
- MAC word length: 25 bits
- Optimal word length configuration determined through SNR analysis

## Pipeline Implementation

To minimize critical path delay, registers were inserted between adders to implement a pipeline structure. This resulted in:
- Certain output latency
- Increased shift register overhead
- Circuit capability to support higher clock frequencies

## Performance Analysis

Comparison between the original circuit and pipelined circuit under 50MHz clock constraint:
- No timing violations in either circuit
- Pipelined circuit used more hardware resources (especially flip-flops)
- After voltage supply adjustment, power consumption of both circuits decreased significantly

## Energy Consumption Comparison

| Circuit Type | Typical Power | Power After Voltage Adjustment | Notes |
|----------|---------|--------------|------|
| Original Circuit | 0.156 W | 0.146 W | Baseline |
| Pipelined Circuit | 0.158 W | 0.147 W | Slightly higher power |

> Note: Although the pipelined circuit allows for higher clock frequencies, the actual power consumption is slightly higher than the original circuit due to the overhead of additional flip-flops.

## Conclusions and Future Improvements

Experimental results show that while the pipeline design can increase maximum operating frequency, it failed to achieve the expected goal of reducing energy consumption due to the additional overhead from flip-flops. This indicates that when optimizing DSP circuits for specific FPGAs, a more comprehensive trade-off between speed, area, and power is needed.

Potential future improvements:
- Try partial pipelining to balance delay and resource usage
- Explore optimal combinations of voltage scaling and frequency scaling
- Consider alternative filter structures, such as half-band filters or polyphase filters

## Tools Used

- Xilinx Vivado Design Suite
- MATLAB for filter design and analysis
