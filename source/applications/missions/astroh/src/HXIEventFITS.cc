#include "HXIEventFITS.hh"

namespace
{

void fillFlagArray(uint64_t flags, int shift, int size, uint8_t* array)
{
  for (int i = 0; i<size; i++) {
    array[i] = (flags >> (shift + i)) & 0x1u;
  }
}

uint64_t convertFlags(uint8_t* array)
{
  const int size = 64;
  uint64_t flags = 0ul;
  for (int i = 0; i<size; i++) {
    if (array[i]) {
      flags |= (0x1ul << i);
    }
  }
  return flags;
}

} // anonymous namespace

namespace astroh {
namespace hxi {

using namespace cfitsio;

EventFITSIOHelper::EventFITSIOHelper()
{
}

EventFITSIOHelper::~EventFITSIOHelper() = default;

void EventFITSIOHelper::fillEvent(const hxi::Event& event)
{
  TIME_[0] = event.getTime();
  S_TIME_[0] = event.getSTime();
  ADU_CNT_[0] = event.getADUCount();
  L32TI_[0] = event.getL32TI();
  OCCURRENCE_ID_[0] = event.getOccurrenceID();
  LOCAL_TIME_[0] = event.getLocalTime();
  CATEGORY_[0] = event.getCategory();
  const uint32_t flags = event.getFlags().get();
  fillFlagArray(flags,  0, 32, FLAGS_.data());
  fillFlagArray(flags, 26,  5, FLAG_SEU_.data());
  fillFlagArray(flags, 20,  5, FLAG_LCHK_.data());
  FLAG_TRIG_[0] = static_cast<uint8_t>((flags>>12)&0xFFu);
  fillFlagArray(flags,  4,  8, FLAG_TRIGPAT_.data());
  fillFlagArray(flags,  2,  2, FLAG_HITPAT_.data());
  fillFlagArray(flags,  0,  2, FLAG_FASTBGO_.data());
  LIVETIME_[0] = event.getLiveTime();
  NUM_ASIC_[0] = event.getNumberOfHitASICs();
  copyVectorToArray(event.getRawASICData(), RAW_ASIC_DATA_);
  PROC_STATUS_[0] = event.getProcessStatus();
  STATUS_[0] = event.getStatus();
  
  copyVectorToArray(event.getASICIDVector(), ASIC_ID_);
  copyVectorToArray(event.getASICIDRemappedVector(), ASIC_ID_RMAP_);
  copyVectorToArray(event.getChipDataBitVector(), ASIC_CHIP_);
  copyVectorToArray(event.getTriggerVector(), ASIC_TRIG_);
  copyVectorToArray(event.getSEUVector(), ASIC_SEU_);
  copyVectorToArray(event.getChannelDataBitVector(), READOUT_FLAG_);
  copyVectorToArray(event.getNumberOfHitChannelsVector(), NUM_READOUT_);
  copyVectorToArray(event.getReferenceLevelVector(), ASIC_REF_);
  copyVectorToArray(event.getCommonModeNoiseVector(), ASIC_CMN_);
  
  copyVectorToArray(event.getReadoutASICIDVector(), READOUT_ASIC_ID_);
  copyVectorToArray(event.getReadoutChannelIDVector(), READOUT_ID_);
  copyVectorToArray(event.getReadoutChannelIDRemappedVector(), READOUT_ID_RMAP_);
  copyVectorToArray(event.getPHAVector(), PHA_);
  copyVectorToArray(event.getEPIVector(), EPI_);

  fitsfile* fits = fitsFile_;
  int status = 0;
  const long int row = rowIndex_;
  const long int elem = 1;
  fits_write_col(fits, TDOUBLE,   1, row, elem,  1, TIME_.data(),           &status);
  fits_write_col(fits, TDOUBLE,   2, row, elem,  1, S_TIME_.data(),         &status);
  fits_write_col(fits, TBYTE,     3, row, elem,  1, ADU_CNT_.data(),        &status);
  fits_write_col(fits, TUINT,     4, row, elem,  1, L32TI_.data(),          &status);
  fits_write_col(fits, TINT32BIT, 5, row, elem,  1, OCCURRENCE_ID_.data(),  &status);
  fits_write_col(fits, TUINT,     6, row, elem,  1, LOCAL_TIME_.data(),     &status);
  fits_write_col(fits, TBYTE,     7, row, elem,  1, CATEGORY_.data(),       &status);
  fits_write_col(fits, TBIT,      8, row, elem, 32, FLAGS_.data(),          &status);
  fits_write_col(fits, TBIT,      9, row, elem,  5, FLAG_SEU_.data(),       &status);
  fits_write_col(fits, TBIT,     10, row, elem,  5, FLAG_LCHK_.data(),      &status);
  fits_write_col(fits, TBYTE,    11, row, elem,  1, FLAG_TRIG_.data(),      &status);
  fits_write_col(fits, TBIT,     12, row, elem,  8, FLAG_TRIGPAT_.data(),   &status);
  fits_write_col(fits, TBIT,     13, row, elem,  2, FLAG_HITPAT_.data(),    &status);
  fits_write_col(fits, TBIT,     14, row, elem,  2, FLAG_FASTBGO_.data(),   &status);
  fits_write_col(fits, TUINT,    15, row, elem,  1, LIVETIME_.data(),       &status);
  fits_write_col(fits, TBYTE,    16, row, elem,  1, NUM_ASIC_.data(),       &status);

  const std::size_t rawASICDataLength = event.getRawASICData().size();
  fits_write_col(fits, TBYTE,    17, row, elem, rawASICDataLength, RAW_ASIC_DATA_.data(), &status);

  fits_write_col(fits, TUINT,    18, row, elem,  1, PROC_STATUS_.data(),    &status);
  fits_write_col(fits, TBYTE,    19, row, elem,  1, STATUS_.data(),         &status);

  const std::size_t ASICDataLength = event.LengthOfASICData();
  fits_write_col(fits, TBYTE,    20, row, elem, ASICDataLength, ASIC_ID_.data(),      &status);
  fits_write_col(fits, TBYTE,    21, row, elem, ASICDataLength, ASIC_ID_RMAP_.data(), &status);
  fits_write_col(fits, TBYTE,    22, row, elem, ASICDataLength, ASIC_CHIP_.data(),    &status);
  fits_write_col(fits, TBYTE,    23, row, elem, ASICDataLength, ASIC_TRIG_.data(),    &status);
  fits_write_col(fits, TBYTE,    24, row, elem, ASICDataLength, ASIC_SEU_.data(),     &status);
  fits_write_col(fits, TUINT,    25, row, elem, ASICDataLength, READOUT_FLAG_.data(), &status);
  fits_write_col(fits, TSHORT,   26, row, elem, ASICDataLength, NUM_READOUT_.data(),  &status);
  fits_write_col(fits, TSHORT,   27, row, elem, ASICDataLength, ASIC_REF_.data(),     &status);
  fits_write_col(fits, TSHORT,   28, row, elem, ASICDataLength, ASIC_CMN_.data(),     &status);

  const std::size_t ReadoutDataLength = event.LengthOfReadoutData();
  fits_write_col(fits, TBYTE,    29, row, elem, ReadoutDataLength, READOUT_ASIC_ID_.data(), &status);
  fits_write_col(fits, TBYTE,    30, row, elem, ReadoutDataLength, READOUT_ID_.data(),      &status);
  fits_write_col(fits, TSHORT,   31, row, elem, ReadoutDataLength, READOUT_ID_RMAP_.data(), &status);
  fits_write_col(fits, TSHORT,   32, row, elem, ReadoutDataLength, PHA_.data(),             &status);
  fits_write_col(fits, TFLOAT,   33, row, elem, ReadoutDataLength, EPI_.data(),             &status);

  rowIndex_++;
}

void EventFITSIOHelper::restoreEvent(long int row, hxi::Event& event)
{
  fitsfile* fits = fitsFile_;
  int anynul = 0;
  int status = 0;
  const long int elem = 1;

  fits_read_col(fits, TDOUBLE,   1, row, elem,  1, NULL, TIME_.data(),          &anynul, &status);
  fits_read_col(fits, TDOUBLE,   2, row, elem,  1, NULL, S_TIME_.data(),        &anynul, &status);
  fits_read_col(fits, TBYTE,     3, row, elem,  1, NULL, ADU_CNT_.data(),       &anynul, &status);
  fits_read_col(fits, TUINT,     4, row, elem,  1, NULL, L32TI_.data(),         &anynul, &status);
  fits_read_col(fits, TINT32BIT, 5, row, elem,  1, NULL, OCCURRENCE_ID_.data(), &anynul, &status);
  fits_read_col(fits, TUINT,     6, row, elem,  1, NULL, LOCAL_TIME_.data(),    &anynul, &status);
  fits_read_col(fits, TBYTE,     7, row, elem,  1, NULL, CATEGORY_.data(),      &anynul, &status);
  fits_read_col(fits, TBIT,      8, row, elem, 32, NULL, FLAGS_.data(),         &anynul, &status);
  fits_read_col(fits, TUINT,    15, row, elem,  1, NULL, LIVETIME_.data(),      &anynul, &status);
  fits_read_col(fits, TBYTE,    16, row, elem,  1, NULL, NUM_ASIC_.data(),      &anynul, &status);

  long int rawASICDataLength(0), rawASICDataOffset(0);
  fits_read_descript(fits, 17, row, &rawASICDataLength, &rawASICDataOffset, &status);
  fits_read_col(fits, TBYTE,    17, row, elem, rawASICDataLength, NULL, RAW_ASIC_DATA_.data(), &anynul, &status);

  fits_read_col(fits, TUINT,    18, row, elem,  1, NULL, PROC_STATUS_.data(), &anynul, &status);
  fits_read_col(fits, TBYTE,    19, row, elem,  1, NULL, STATUS_.data(),      &anynul, &status);

  long int ASICDataLength(0), ASICDataOffset(0);
  fits_read_descript(fits, 20, row, &ASICDataLength, &ASICDataOffset, &status);
  fits_read_col(fits, TBYTE,     20, row, elem, ASICDataLength, NULL, ASIC_ID_.data(),      &anynul, &status);
  fits_read_col(fits, TBYTE,     21, row, elem, ASICDataLength, NULL, ASIC_ID_RMAP_.data(), &anynul, &status);
  fits_read_col(fits, TBYTE,     22, row, elem, ASICDataLength, NULL, ASIC_CHIP_.data(),    &anynul, &status);
  fits_read_col(fits, TBYTE,     23, row, elem, ASICDataLength, NULL, ASIC_TRIG_.data(),    &anynul, &status);
  fits_read_col(fits, TBYTE,     24, row, elem, ASICDataLength, NULL, ASIC_SEU_.data(),     &anynul, &status);
  fits_read_col(fits, TLONGLONG, 25, row, elem, ASICDataLength, NULL, READOUT_FLAG_.data(), &anynul, &status);
  fits_read_col(fits, TSHORT,    26, row, elem, ASICDataLength, NULL, NUM_READOUT_.data(),  &anynul, &status);
  fits_read_col(fits, TSHORT,    27, row, elem, ASICDataLength, NULL, ASIC_REF_.data(),     &anynul, &status);
  fits_read_col(fits, TSHORT,    28, row, elem, ASICDataLength, NULL, ASIC_CMN_.data(),     &anynul, &status);
  
  long int ReadoutDataLength(0), ReadoutDataOffset(0);
  fits_read_descript(fits, 29, row, &ReadoutDataLength, &ReadoutDataOffset, &status);
  fits_read_col(fits, TBYTE,    29, row, elem, ReadoutDataLength, NULL, READOUT_ASIC_ID_.data(), &anynul, &status);
  fits_read_col(fits, TBYTE,    30, row, elem, ReadoutDataLength, NULL, READOUT_ID_.data(),       &anynul, &status);
  fits_read_col(fits, TSHORT,   31, row, elem, ReadoutDataLength, NULL, READOUT_ID_RMAP_.data(), &anynul, &status);
  fits_read_col(fits, TSHORT,   32, row, elem, ReadoutDataLength, NULL, PHA_.data(),             &anynul, &status);
  fits_read_col(fits, TFLOAT,   33, row, elem, ReadoutDataLength, NULL, EPI_.data(),             &anynul, &status);

  event.setTime(TIME_[0]);
  event.setSTime(S_TIME_[0]);
  event.setADUCount(ADU_CNT_[0]);
  event.setL32TI(L32TI_[0]);
  event.setOccurrenceID(OCCURRENCE_ID_[0]);
  event.setLocalTime(LOCAL_TIME_[0]);
  event.setCategory(CATEGORY_[0]);
  event.setFlags(convertFlags(FLAGS_.data()));
  event.setLiveTime(LIVETIME_[0]);
  event.setNumberOfHitASICs(NUM_ASIC_[0]);
  event.setRawASICData(std::vector<uint8_t>(RAW_ASIC_DATA_.begin(),
                                            RAW_ASIC_DATA_.begin()+rawASICDataLength));
  event.setProcessStatus(PROC_STATUS_[0]);
  event.setStatus(STATUS_[0]);
  
  const int NumASICs = ASICDataLength;
    
  event.reserveASICData(NumASICs);
  for (int i=0; i<NumASICs; i++) {
    const uint8_t ASIC_ID = ASIC_ID_[i];
    const uint8_t ASIC_ID_remapped = ASIC_ID_RMAP_[i];
    const uint8_t chipDataBit = ASIC_CHIP_[i];
    const uint8_t trigger = ASIC_TRIG_[i];
    const uint8_t SEU = ASIC_SEU_[i];
    const uint32_t channelDataBit = READOUT_FLAG_[i];
    const int16_t numberOfHitChannels = NUM_READOUT_[i];
    const int16_t referenceLevel = ASIC_REF_[i];
    const int16_t commonModeNoise = ASIC_CMN_[i];
    event.pushASICData(ASIC_ID,
                       ASIC_ID_remapped,
                       chipDataBit,
                       trigger,
                       SEU,
                       channelDataBit,
                       numberOfHitChannels,
                       referenceLevel,
                       commonModeNoise);
  }
  
  const int NumReadouts = ReadoutDataLength;
  event.reserveReadoutData(NumReadouts);
  for (int i=0; i<NumReadouts; i++) {
    const uint8_t ASIC_ID = READOUT_ASIC_ID_[i];
    const uint8_t channelID = READOUT_ID_[i];
    const int16_t channelID_remapped = READOUT_ID_RMAP_[i];
    const int16_t PHA = PHA_[i];
    const float EPI = EPI_[i];
    event.pushReadoutData(ASIC_ID,
                          channelID,
                          channelID_remapped,
                          PHA,
                          EPI);
  }
}

std::shared_ptr<hxi::Event> EventFITSIOHelper::getEvent(long int row)
{
  std::shared_ptr<hxi::Event> event = std::make_shared<hxi::Event>();
  restoreEvent(row, *event);
  return event;
}

bool EventFITSIOHelper::createFITSFile(const std::string& filename)
{
  int fitsStatus = 0;
  fits_create_file(&fitsFile_, filename.c_str(), &fitsStatus);
  fits_report_error(stderr, fitsStatus);
  if (fitsStatus != 0) { return false; }
  return true;
}

void EventFITSIOHelper::initializeFITSTable(long int numberOfRows)
{
  int fitsStatus = 0;
  
  long naxis = 1;
  long naxes[1] = { 0 };
  fits_create_img(fitsFile_, USHORT_IMG, naxis, naxes, &fitsStatus);
  fits_report_error(stderr, fitsStatus);
  
  const int NumColumns = 37;
  const int TableType = BINARY_TBL;
  const int tfields = NumColumns;
  
  char* ttype[NumColumns] = {
    (char*) "TIME",            //  1
    (char*) "S_TIME",          //  2
    (char*) "ADU_CNT",         //  3
    (char*) "L32TI",           //  4
    (char*) "OCCURRENCE_ID",   //  5
    (char*) "LOCAL_TIME",      //  6
    (char*) "CATEGORY",        //  7
    (char*) "FLAGS",           //  8
    (char*) "FLAG_SEU",        //  9
    (char*) "FLAG_LCHK",       // 10
    (char*) "FLAG_TRIG",       // 11
    (char*) "FLAG_TRIGPAT",    // 12
    (char*) "FLAG_HITPAT",     // 13
    (char*) "FLAG_FASTBGO",    // 14
    (char*) "LIVETIME",        // 15
    (char*) "NUM_ASIC",        // 16
    (char*) "RAW_ASIC_DATA",   // 17
    (char*) "PROC_STATUS",     // 18
    (char*) "STATUS",          // 19
    (char*) "ASIC_ID",         // 20
    (char*) "ASIC_ID_RMAP",    // 21
    (char*) "ASIC_CHIP",       // 22
    (char*) "ASIC_TRIG",       // 23
    (char*) "ASIC_SEU",        // 24
    (char*) "READOUT_FLAG",    // 25
    (char*) "NUM_READOUT",     // 26
    (char*) "ASIC_REF",        // 27
    (char*) "ASIC_CMN",        // 28
    (char*) "READOUT_ASIC_ID", // 29
    (char*) "READOUT_ID",      // 30
    (char*) "READOUT_ID_RMAP", // 31
    (char*) "PHA",             // 32
    (char*) "EPI",             // 33
  };
  
  char* tform[NumColumns] = {
    (char*) "1D", /* TIME */
    (char*) "1D", /* S_TIME */
    (char*) "1B", /* ADU_CNT */
    (char*) "1V", /* L32TI */
    (char*) "1J", /* OCCURRENCE_ID */
    (char*) "1V", /* LOCAL_TIME */
    (char*) "1B", /* CATEGORY */
    (char*) "32X", /* FLAGS */
    (char*) "5X", /* FLAG_SEU */
    (char*) "5X", /* FLAG_LCHK */
    (char*) "1B", /* FLAG_TRIG */
    (char*) "8X", /* FLAG_TRIGPAT */
    (char*) "2X", /* FLAG_HITPAT */
    (char*) "2X", /* FLAG_FASTBGO */
    (char*) "1V", /* LIVETIME */
    (char*) "1B", /* NUM_ASIC */
    (char*) "1PB(2000)", /* RAW_ASIC_DATA */
    (char*) "32X", /* PROC_STATUS */
    (char*) "8X", /* STATUS */
    (char*) "1PB(40)", /* ASIC_ID */
    (char*) "1PB(40)", /* ASIC_ID_RMAP */
    (char*) "1PX(40)", /* ASIC_CHIP */
    (char*) "1PX(40)", /* ASIC_TRIG */
    (char*) "1PX(40)", /* ASIC_SEU */
    (char*) "1PK(40)", /* READOUT_FLAG */
    (char*) "1PI(40)", /* NUM_READOUT */
    (char*) "1PI(40)", /* ASIC_REF */
    (char*) "1PI(40)", /* ASIC_CMN */
    (char*) "1PB(1280)", /* READOUT_ASIC_ID */
    (char*) "1PB(1280)", /* READOUT_ID */
    (char*) "1PI(1280)", /* READOUT_ID_RMAP */
    (char*) "1PI(1280)", /* PHA */
    (char*) "1PE(1280)", /* EPI */
  };
  
  char* tunit[NumColumns] = {
    (char*) "", //  1
    (char*) "", //  2
    (char*) "", //  3
    (char*) "", //  4
    (char*) "", //  5
    (char*) "", //  6
    (char*) "", //  7
    (char*) "", //  8
    (char*) "", //  9
    (char*) "", // 10
    (char*) "", // 11
    (char*) "", // 12
    (char*) "", // 13
    (char*) "", // 14
    (char*) "", // 15
    (char*) "", // 16
    (char*) "", // 17
    (char*) "", // 18
    (char*) "", // 19
    (char*) "", // 20
    (char*) "", // 21
    (char*) "", // 22
    (char*) "", // 23
    (char*) "", // 24
    (char*) "", // 25
    (char*) "", // 26
    (char*) "", // 27
    (char*) "", // 28
    (char*) "", // 29
    (char*) "", // 30
    (char*) "", // 31
    (char*) "", // 32
    (char*) "", // 33
  };
  
  char extname[] = "EVENTS";
  fits_create_tbl(fitsFile_, TableType, numberOfRows, tfields, ttype, tform, tunit, extname, &fitsStatus);
  fits_report_error(stderr, fitsStatus);

  // Move to the table HDU.
  fits_movabs_hdu(fitsFile_, 2, NULL, &fitsStatus);
  fits_report_error(stderr, fitsStatus);
  
#define FILL_FITS_HEADER_ 0
#if FILL_FITS_HEADER_
  // Register the common header keywords.
  AHCommonKeys commonHeaderKeywords;
  setCommonKeys(&commonHeaderKeywords);
  commonHeaderKeywords.instrume = const_cast<char*>(instrumentName.c_str());
  commonHeaderKeywords.detnam = const_cast<char*>(detectorName.c_str());
  commonHeaderKeywords.datamode = const_cast<char*>(dataMode.c_str());
  
  // Write primary header keywords
  fits_movabs_hdu(file, 1, NULL, &fitsStatus);
  writePrimaryHeader(file, &commonHeaderKeywords);
  
  // Write event extension header keywords
  fits_movabs_hdu(file, 2, NULL, &fitsStatus);
  writeEventHeader(file, &commonHeaderKeywords);

  // Expand header keyword space.
  const int nAdditionalKeywords=100;
	fits_set_hdrsize(file, nAdditionalKeywords, &fitsStatus );
	this->reportErrorThenQuitIfError(fitsStatus, __func__);
#endif /* FILL_FITS_HEADER_ */
}

bool EventFITSIOHelper::openFITSFile(const std::string& filename)
{
  int fitsStatus = 0;
  fits_open_table(&fitsFile_, filename.c_str(), READONLY, &fitsStatus);
  fits_report_error(stderr, fitsStatus);
  if (fitsStatus != 0) { return false; }
  return true;
}

long int EventFITSIOHelper::NumberOfRows()
{
  long nrows = 0;
  int fitsStatus = 0;
  fits_get_num_rows(fitsFile_, &nrows, &fitsStatus);
  return nrows;
}

void EventFITSIOHelper::closeFITSFile()
{
  int status = 0;
  fits_close_file(fitsFile_, &status);
  fits_report_error(stderr, status);
}

/********************************
 * EventFITSWriter
 *******************************/
EventFITSWriter::EventFITSWriter()
  : io_(new EventFITSIOHelper)
{
}
  
EventFITSWriter::~EventFITSWriter() = default;

bool EventFITSWriter::open(const std::string& filename)
{
  bool status = true;
  status = io_->createFITSFile(filename);
  if (!status) { return status; }
  io_->initializeFITSTable();
  return status;
}

void EventFITSWriter::fillEvent(const hxi::Event& event)
{
  io_->fillEvent(event);
}

void EventFITSWriter::close()
{
  io_->closeFITSFile();
}


/********************************
 * EventFITSReader
 *******************************/

EventFITSReader::EventFITSReader()
  : io_(new EventFITSIOHelper)
{
}

EventFITSReader::~EventFITSReader() = default;

bool EventFITSReader::open(const std::string& filename)
{
  return io_->openFITSFile(filename);
}

long int EventFITSReader::NumberOfRows()
{
  return io_->NumberOfRows();
}
  
void EventFITSReader::restoreEvent(long int row, hxi::Event& event)
{
  io_->restoreEvent(row, event);
}
  
std::shared_ptr<hxi::Event> EventFITSReader::getEvent(long int row)
{
  return io_->getEvent(row);
}

void EventFITSReader::close()
{
  io_->closeFITSFile();
}

} // namespace hxi
} // namespace astroh
