#include "nw4r/snd/BankFile.h"

/* Original source:
 * kiwi515/ogws
 * src/nw4r/snd/snd_BankFile.cpp
 */

/*******************************************************************************
 * headers
 */

#include <macros.h> // NW4R_VERSION
#include <types.h>

#include "nw4r/snd/Util.h"
#include "nw4r/snd/WaveArchive.h" // WaveArchiveReader
#include "nw4r/snd/WaveFile.h"

#include "nw4r/ut/binaryFileFormat.h" // ut::BinaryFileHeader
#include "nw4r/ut/inlines.h"

#include "nw4r/NW4RAssert.hpp"

/*******************************************************************************
 * local function declarations
 */

namespace nw4r { namespace snd { namespace detail
{
	inline byte_t ReadByte(const void *address)
	{
		return *static_cast<const byte_t *>(address);
	}
}}} // namespace nw4r::snd::detail

/*******************************************************************************
 * functions
 */

namespace nw4r { namespace snd { namespace detail {

bool BankFileReader::IsValidFileHeader(const void *bankData)
{
	const ut::BinaryFileHeader *pFileHeader =
		static_cast<const ut::BinaryFileHeader *>(bankData);

	NW4RAssertMessage_Line(
		59, pFileHeader->signature == SIGNATURE,
		"invalid file signature. bank data is not available.");
	if (pFileHeader->signature != SIGNATURE)
		return false;

	NW4RAssertMessage_Line(67, pFileHeader->version >= NW4R_VERSION(1, 0),
	                       "bank file is not supported version.\n  please "
	                       "reconvert file using new version tools.\n");
	if (pFileHeader->version < NW4R_VERSION(1, 0))
		return false;

	NW4RAssertMessage_Line(73, pFileHeader->version <= VERSION,
	                       "bank file is not supported version.\n  please "
	                       "reconvert file using new version tools.\n");
	if (pFileHeader->version > VERSION)
		return false;

	return true;
}

BankFileReader::BankFileReader(const void *bankData):
	mHeader		(nullptr),
	mDataBlock	(nullptr),
	mWaveBlock	(nullptr)
{
	NW4RAssertPointerNonnull_Line(93, bankData);

	if (!IsValidFileHeader(bankData))
		return;

	mHeader = static_cast<const BankFile::Header *>(bankData);

	if (mHeader->dataBlockOffset)
	{
		mDataBlock = static_cast<const BankFile::DataBlock *>(
			ut::AddOffsetToPtr(mHeader, mHeader->dataBlockOffset));

		NW4RAssert_Line(105, mDataBlock->blockHeader.kind
		                         == BankFile::SIGNATURE_DATA_BLOCK);
	}

	if (mHeader->waveBlockOffset)
	{
		mWaveBlock = static_cast<const BankFile::WaveBlock *>(
			ut::AddOffsetToPtr(mHeader, mHeader->waveBlockOffset));

		NW4RAssert_Line(113, mWaveBlock->blockHeader.kind
		                         == BankFile::SIGNATURE_WAVE_BLOCK);
	}
}

const BankFile::InstParam *BankFileReader::GetInstParam(int prgNo, int key,
                                                        int velocity) const
{
	NW4RAssertPointerNonnull_Line(135, mHeader);
	if (!mHeader)
		return nullptr;

	if (prgNo < 0 || prgNo >= static_cast<int>(mDataBlock->instTable.count))
		return nullptr;

	const BankFile::DataRegion *ref = &mDataBlock->instTable.items[prgNo];

	if (ref->dataType == 4)
		return nullptr;

	if (ref->dataType != 1)
	{
		ref = GetReferenceToSubRegion(ref, key);
		if (!ref)
			return nullptr;
	}

	if (ref->dataType == 4)
		return nullptr;

	if (ref->dataType != 1)
	{
		ref = GetReferenceToSubRegion(ref, velocity);
		if (!ref)
			return nullptr;
	}

	if (ref->dataType != 1)
		return nullptr;

	const BankFile::InstParam *instParam =
		Util::GetDataRefAddress1(*ref, &mDataBlock->instTable);

	return instParam;
}

bool BankFileReader::ReadInstInfo(InstInfo *instInfo, int prgNo, int key,
                                  int velocity) const
{
	NW4RAssertPointerNonnull_Line(188, instInfo);

	const BankFile::InstParam *instParam = GetInstParam(prgNo, key, velocity);
	if (!instParam)
		return false;

	if (instParam->waveDataLocationType
	    == InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_INDEX)
	{
		if (instParam->waveIndex < 0)
			return false;

		instInfo->waveDataLocation.type =
			InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_INDEX;
		instInfo->waveDataLocation.index = instParam->waveIndex;
	}
	else if (instParam->waveDataLocationType
	         == InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_ADDRESS)
	{
		if (!instParam->waveInfoAddress)
			return false;

		instInfo->waveDataLocation.type =
			InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_ADDRESS;
		instInfo->waveDataLocation.address = instParam->waveInfoAddress;
	}
	else if (instParam->waveDataLocationType
	         == InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_CALLBACK)
	{
		if (!instParam->waveDataLocationCallback)
			return false;

		instInfo->waveDataLocation.type =
			InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_CALLBACK;
		instInfo->waveDataLocation.callback =
			instParam->waveDataLocationCallback;
	}
	else
	{
		NW4RPanicMessage_Line(210, "Invalid waveDataLocationType %d",
		                      instParam->waveDataLocationType);
		return false;
	}

	instInfo->attack		= instParam->attack;
	instInfo->hold			= instParam->hold;
	instInfo->decay			= instParam->decay;
	instInfo->sustain		= instParam->sustain;
	instInfo->release		= instParam->release;
	instInfo->originalKey	= instParam->originalKey;
	instInfo->pan			= instParam->pan;

	if (mHeader->fileHeader.version >= NW4R_VERSION(1, 1))
	{
		instInfo->volume	= instParam->volume;
		instInfo->tune		= instParam->tune;
	}
	else
	{
		instInfo->volume	= 127;
		instInfo->tune		= 1.0f;
	}

	switch (instParam->noteOffType)
	{
	case 0:
		instInfo->noteOffType = InstInfo::NOTE_OFF_TYPE_RELEASE;
		break;

	case 1:
		instInfo->noteOffType = InstInfo::NOTE_OFF_TYPE_IGNORE;
		break;

	default:
		NW4RPanicMessage_Line(240, "Invalid noteOffType %d",
		                      instParam->noteOffType);
		return false;
	}

	instInfo->alternateAssign = instParam->alternateAssign;

	return true;
}

const BankFile::DataRegion *BankFileReader::GetReferenceToSubRegion(
	const BankFile::DataRegion *ref, int splitKey) const
{
	const BankFile::DataRegion *pSub = nullptr;

	switch (ref->dataType)
	{
	case 0:
		break;

	case 1:
		pSub = ref;
		break;

	case 2:
	{
		const BankFile::RangeTable *table =
			Util::GetDataRefAddress2(*ref, &mDataBlock->instTable);

		if (!table)
			return nullptr;

		int index = 0;
		while (splitKey > ReadByte(table->key + index))
		{
			if (++index >= table->tableSize)
				return nullptr;
		}

		u32 refOffset = 8ul * index + ut::RoundUp(table->tableSize + 1, 4);

		/* TODO: how to properly match this call? (the arguments to
		 * AddOffsetToPtr are supposed to be the other way around)
		 */
		pSub = static_cast<const BankFile::DataRegion *>(ut::AddOffsetToPtr(
			reinterpret_cast<const void *>(refOffset),
			reinterpret_cast<u32>(table)));
	}
		break;

	case 3:
	{
		const BankFile::IndexTable *table =
			Util::GetDataRefAddress3(*ref, &mDataBlock->instTable);

		if (!table)
			return nullptr;

		if (splitKey < table->min || splitKey > table->max)
			return nullptr;

		pSub = reinterpret_cast<const BankFile::DataRegion *>(
			table->ref
			+ (splitKey - table->min) * sizeof(BankFile::DataRegion));
	}
		break;
	}

	return pSub;
}

bool BankFileReader::ReadWaveInfo(
	WaveInfo *waveParam, const InstInfo::WaveDataLocation &waveDataLocation,
	const void *waveDataAddress, const WaveInfo **waveInfoAddress) const
{
	NW4RAssertPointerNonnull_Line(331, waveParam);

	if (waveInfoAddress)
		*waveInfoAddress = nullptr;

	if (!mHeader)
		return false;

	if (waveDataLocation.type
	    == InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_INDEX)
	{
		int waveIndex = waveDataLocation.index;

		if (!mWaveBlock)
		{
			WaveArchiveReader waveArchiveReader(waveDataAddress);
			const WaveFile::FileHeader *fileHeader =
				waveArchiveReader.GetWaveFile(waveIndex);

			if (!fileHeader)
				return false;

			WaveFileReader waveFileReader(fileHeader);
			return waveFileReader.ReadWaveInfo(waveParam);
		}
		else if (waveIndex >= mWaveBlock->waveInfoTable.count)
		{
			return false;
		}
		else
		{
			const WaveFile::WaveInfo *waveInfo = Util::GetDataRefAddress0(
				mWaveBlock->waveInfoTable.items[waveIndex],
				&mWaveBlock->waveInfoTable);

			if (!waveInfo)
				return false;

			WaveFileReader waveFileReader(waveInfo);
			return waveFileReader.ReadWaveInfo(waveParam, waveDataAddress);
		}
	}
	else if (waveDataLocation.type
	         == InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_ADDRESS)
	{
		if (!waveDataLocation.address)
			return false;

		if (waveInfoAddress)
			*waveInfoAddress = waveDataLocation.address;

		*waveParam = *waveDataLocation.address;
		return true;
	}
	else if (waveDataLocation.type
	         == InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_CALLBACK)
	{
		if (!waveDataLocation.callback)
			return false;

		WaveInfo *waveInfo = waveDataLocation.callback->GetWaveInfo();
		if (!waveInfo)
			return false;

		if (waveInfoAddress)
			*waveInfoAddress = waveInfo;

		*waveParam = *waveInfo;
		return true;
	}
	else
	{
		NW4RPanicMessage_Line(389, "Invalid waveDataLocation.type %d",
		                      waveDataLocation.type);
		return false;
	}
}

}}} // namespace nw4r::snd::detail
