# IO Setup

This is a brief discussion on the IO processing architecture.

## LiDAR

The lidar sends somewhat frequent scans over UART, and as such cannot be simply polled. To make this efficient, we
use interrupts in the background to read each byte into an intermediate buffer. When a packet length has been received,
we then copy it over into the driver. Because the packet may be misaligned (the header byte of the packet is not the
first byte), we need to double buffer the scan. This works by aligning the first scan into the first buffer, which
means that the next packet read must overflow that buffer. To solve this, we copy the overflowed bytes into the double
buffer, which is continually written into as long as the first buffer is full. When the main loop finally reads a scan,
we process the first buffer and then swap them, allowing for another scan to be read into the second buffer.

This setup allows for us to read packets from the LiDAR without ever dropping a scan, since the packet bytes are read in
an interrupt. This holds true unless:

- Interrupts are disabled for a significant period of time
- Interrupts of a higher priority starve the UART interrupt
- Too much processing is done in the UART IRQ, which in effect starves itself
- The time between using scans is long enough for 2 entire packets to be received

In theory, DMA could improve this efficiency. I have yet to get it working though. 

## Logging

Logging is done using USART3, which is connected to the st-link debugger USB. Normally this is the Arduino Serial struct,
but we had to disable that module in order to work as low level as we needed for the LiDAR. Because of this, we implement
our own minimal Serial equivalent. This class allows for asynchronous printf logging via interrupts. It does this by 
first taking all printf strings and storing them in a FIFO queue. This queue is then transmitted over USART3 one by one
via an interrupt that fires after the last has finished transmitting. This allows for logging to not block the main loop,
at the cost of latency and memory.

This has the following restrictions:
- Messages cannot be sent when interrupts are disabled
- The FIFO has a limit, and sending too many messages too fast can cause them to start to drop