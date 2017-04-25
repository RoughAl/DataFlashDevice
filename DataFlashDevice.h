/* mbed Microcontroller Library
 * Copyright (c) 2016 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_SPIDF_BLOCK_DEVICE_H
#define MBED_SPIDF_BLOCK_DEVICE_H

#include <mbed.h>
#include "BlockDevice.h"

/** BlockDevice for SPI based flash devices
 *  such as the MX25R or SST26F016B
 *
 *  @code
 *  #include "mbed.h"
 *  #include "DataFlashDevice.h"
 *
 *  // Create mx25r on SPI bus with PTE5 as chip select
 *  DataFlashDevice mx25r(PTE2, PTE4, PTE1, PTE5);
 *
 *  int main() {
 *      printf("mx25r test\n");
 *      mx52r.init();
 *      printf("mx25r size: %llu\n", mx25r.size());
 *      printf("mx25r read size: %llu\n", mx25r.get_read_size());
 *      printf("mx25r program size: %llu\n", mx25r.get_program_size());
 *      printf("mx25r erase size: %llu\n", mx25r.get_erase_size());
 *
 *      uint8_t *buffer = malloc(mx25r.get_erase_size());
 *      sprintf(buffer, "Hello World!\n");
 *      mx25r.erase(0, mx25r.get_erase_size());
 *      mx25r.program(buffer, 0, mx25r.get_erase_size());
 *      mx25r.read(buffer, 0, mx25r.get_erase_size());
 *      printf("%s", buffer);
 *
 *      mx25r.deinit();
 *  }
 */
class DataFlashDevice: public BlockDevice {
public:
	/** Creates a DataFlashDevice on a SPI bus specified by pins
	 *
	 *  @param mosi     SPI master out, slave in pin
	 *  @param miso     SPI master in, slave out pin
	 *  @param sclk     SPI clock pin
	 *  @param csel     SPI chip select pin
	 *  @param freq     Clock speed of the SPI bus (defaults to 40MHz)
	 */
	DataFlashDevice(PinName mosi, PinName miso, PinName sclk, PinName csel,
			PinName wp, int freq = 40000000);

	/** Initialize a block device
	 *
	 *  @return         0 on success or a negative error code on failure
	 */
	virtual int init();

	/** Deinitialize a block device
	 *
	 *  @return         0 on success or a negative error code on failure
	 */
	virtual int deinit();

	/** Read blocks from a block device
	 *
	 *  @param buffer   Buffer to write blocks to
	 *  @param addr     Address of block to begin reading from
	 *  @param size     Size to read in bytes, must be a multiple of read block size
	 *  @return         0 on success, negative error code on failure
	 */
	virtual int read(void *buffer, bd_addr_t addr, bd_size_t size);

	/** Program blocks to a block device
	 *
	 *  The blocks must have been erased prior to being programmed
	 *
	 *  @param buffer   Buffer of data to write to blocks
	 *  @param addr     Address of block to begin writing to
	 *  @param size     Size to write in bytes, must be a multiple of program block size
	 *  @return         0 on success, negative error code on failure
	 */
	virtual int program(const void *buffer, bd_addr_t addr, bd_size_t size);

	/** Erase blocks on a block device
	 *
	 *  The state of an erased block is undefined until it has been programmed
	 *
	 *  @param addr     Address of block to begin erasing
	 *  @param size     Size to erase in bytes, must be a multiple of erase block size
	 *  @return         0 on success, negative error code on failure
	 */
	virtual int erase(bd_addr_t addr, bd_size_t size);
	
	

	/** Get the size of a readable block
	 *
	 *  @return         Size of a readable block in bytes
	 */
	virtual bd_size_t get_read_size() const;

	/** Get the size of a programable block
	 *
	 *  @return         Size of a programable block in bytes
	 *  @note Must be a multiple of the read size
	 */
	virtual bd_size_t get_program_size() const;

	/** Get the size of a eraseable block
	 *
	 *  @return         Size of a eraseable block in bytes
	 *  @note Must be a multiple of the program size
	 */
	virtual bd_size_t get_erase_size() const;

	/** Get the total size of the underlying device
	 *
	 *  @return         Size of the underlying device in bytes
	 */
	virtual bd_size_t size() const;

private:
	// Master side hardware
	SPI _spi;
	DigitalOut _cs;
	DigitalOut _wp;

	int pages;            // Integer number of pages
	int pagesize;         // page size, in bytes
	int devicesize;       // device size in bytes
	int blocks;           // Number of blocks
	bool _deep_down;       // True = the device is deep down
	bool _deep_down_onoff; // variable for deep power down function (On/Off)

	// Device configuration discovered through sfdp
	bd_size_t _size;

	// read the ID of the device.

	int id(void);
	/** Status register.
	 *
	 * read the status register.
	 */
	int status(void);    // Status register
	
	bool isbusy(void); // report if Flash is busy

	void busy(void); // Wait until Flash is not busy
	
	/** Erase blocks on a block device
	 *
	 *  The state of an erased block is undefined until it has been programmed
	 *
	 *  @param addr     Address of block to begin erasing
	 *  @param size     Size to erase in bytes, must be a multiple of erase block size
	 *  @return         0 on success, negative error code on failure
	 */
	int eraseBlock(bd_addr_t addr, bd_size_t size);

	/** Deep Power Down.
	 *
	 * Remenber that you have to want 35uS after the wake up to use the device.
	 * @param True = Activate and False = Wake Up.
	 */
	void deep_power_down(bool onoff);

	/** Is the device deep power down.
	 *
	 * @return True = Activate and False = Awake.
	 */
	bool is_it_awake(void);
	void _sendaddr(int address);

	// Calculate page address
	int _getpaddr(int);
	//Calculate subpage address 
	int _getbaddr(int);

	void wren(bool en);

	int _sync();

};

#endif  /* MBED_SPIDF_BLOCK_DEVICE_H */
