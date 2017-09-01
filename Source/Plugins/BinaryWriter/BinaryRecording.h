/*
------------------------------------------------------------------

This file is part of the Open Ephys GUI
Copyright (C) 2013 Open Ephys

------------------------------------------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef BINARYRECORDING_H
#define BINARYRECORDING_H

#include <RecordingLib.h>
#include "SequentialBlockFile.h"
#include "NpyFile.h"

namespace BinaryRecordingEngine
{

	class BinaryRecording : public RecordEngine
	{
	public:
		BinaryRecording();
		~BinaryRecording();

		String getEngineID() const override;
		void openFiles(File rootFolder, int experimentNumber, int recordingNumber) override;
		void closeFiles() override;
		void writeData(int writeChannel, int realChannel, const float* buffer, int size) override;
		void writeEvent(int eventIndex, const MidiMessage& event) override;
		void resetChannels() override;
		void addSpikeElectrode(int index, const SpikeChannel* elec) override;
		void writeSpike(int electrodeIndex, const SpikeEvent* spike) override;
		void writeTimestampSyncText(uint16 sourceID, uint16 sourceIdx, int64 timestamp, float, String text) override;
		void setParameter(EngineParameter& parameter) override;

		static RecordEngineManager* getEngineManager();

	private:

		class EventRecording
		{
		public:
			ScopedPointer<NpyFile> mainFile;
			ScopedPointer<NpyFile> timestampFile;
			ScopedPointer<NpyFile> metaDataFile;
			ScopedPointer<NpyFile> channelFile;
			ScopedPointer<NpyFile> extraFile;
		};
		
		enum SpikeMode
		{
			AllInOne = 0,
			SeparateTimestamps = 1,
			AllSeparated = 2
		};

		enum TTLMode
		{
			JointWord = 0,
			SeparateWord = 1,
			NoWord = 2
		};

		enum EventMode
		{
			JointChannel = 0,
			SeparateChannel = 1
		};

		NpyFile* createEventMetadataFile(const MetaDataEventObject* channel, String fileName, DynamicObject* jsonObject);
		void createChannelMetaData(const MetaDataInfoObject* channel, DynamicObject* jsonObject);
		void writeEventMetaData(const MetaDataEvent* event, NpyFile* file);
		void increaseEventCounts(EventRecording* rec);
		static String jsonTypeValue(BaseType type);

		SpikeMode m_spikeMode;
		TTLMode m_TTLMode;
		EventMode m_eventMode;


		HeapBlock<float> m_scaledBuffer;
		HeapBlock<int16> m_intBuffer;
		HeapBlock<int64> m_tsBuffer;
		int m_bufferSize;

		OwnedArray<SequentialBlockFile>  m_DataFiles;
		Array<unsigned int> m_channelIndexes;
		Array<unsigned int> m_fileIndexes;
		OwnedArray<EventRecording> m_eventFiles;
		OwnedArray<EventRecording> m_spikeFiles;
		OwnedArray<NpyFile> m_dataTimestampFiles;
		ScopedPointer<NpyFile> m_syncTextFile;

		Array<unsigned int> m_spikeFileIndexes;
		Array<uint16> m_spikeChannelIndexes;

		int m_recordingNum;
		Array<int64> m_startTS;


		//Compile-time constants
		const int samplesPerBlock{ 4096 };

	};

}

#endif