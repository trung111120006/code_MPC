#!/bin/bash

# =============================================================================
# Script tự động decode file .bin sử dụng mpeg-pcc-tmc2
# Usage: ./auto_decode.sh <bin_file_path> <segment_num> <quality_name>
# Example: ./auto_decode.sh ./download/segment_001_LOW.bin 1 LOW
# =============================================================================

# Cấu hình - THAY ĐỔI ĐƯỜNG DẪN NÀY
TMC2_PATH="/home/backne/Trung/code/code_multiple_version_point_clound/mpeg-pcc-tmc2"
DECODED_OUTPUT_DIR="/home/backne/Trung/code/code_mpc/MPC_libcurl/decoded_output"

# Đường dẫn decoder và tools
DECODER_PATH="$TMC2_PATH/bin/PccAppDecoder"
HDRCONVERT_PATH="$TMC2_PATH/dependencies/HDRTools/build/bin/HDRConvert"

# Config files
CONFIG_COMMON="$TMC2_PATH/cfg/common/ctc-common.cfg"
CONFIG_CONDITION="$TMC2_PATH/cfg/condition/ctc-all-intra.cfg"
CONFIG_SEQUENCE="$TMC2_PATH/cfg/sequence/longdress_vox10.cfg"
CONFIG_RATE="$TMC2_PATH/cfg/rate/ctc-r2.cfg"

# Màu sắc cho output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# =============================================================================
# Nhận tham số từ command line
# =============================================================================
if [ $# -lt 1 ]; then
    echo -e "${RED}Usage: $0 <bin_file_path> [segment_num] [quality_name]${NC}"
    echo -e "${YELLOW}Example: $0 ./download/segment_001_LOW.bin 1 LOW${NC}"
    exit 1
fi

BIN_FILE="$1"
SEGMENT_NUM="${2:-1}"
QUALITY_NAME="${3:-UNKNOWN}"

# =============================================================================
# Kiểm tra file và directories
# =============================================================================
if [ ! -f "$BIN_FILE" ]; then
    echo -e "${RED}[ERROR] Binary file not found: $BIN_FILE${NC}"
    exit 1
fi

if [ ! -f "$DECODER_PATH" ]; then
    echo -e "${RED}[ERROR] Decoder not found: $DECODER_PATH${NC}"
    exit 1
fi

if [ ! -d "$DECODED_OUTPUT_DIR" ]; then
    echo -e "${YELLOW}[INFO] Creating output directory: $DECODED_OUTPUT_DIR${NC}"
    mkdir -p "$DECODED_OUTPUT_DIR"
fi

# Kiểm tra config files
for config in "$CONFIG_COMMON" "$CONFIG_CONDITION" "$CONFIG_SEQUENCE" "$CONFIG_RATE"; do
    if [ ! -f "$config" ]; then
        echo -e "${YELLOW}[WARNING] Config file not found: $config${NC}"
    fi
done

# =============================================================================
# Tạo tên file output
# =============================================================================
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
OUTPUT_PREFIX="$DECODED_OUTPUT_DIR/segment_$(printf '%03d' $SEGMENT_NUM)_${QUALITY_NAME}_${TIMESTAMP}"
OUTPUT_FILE="${OUTPUT_PREFIX}_%04i.ply"

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}  TMC2 Decoder${NC}"
echo -e "${GREEN}========================================${NC}"
echo -e "${BLUE}Input:  $BIN_FILE${NC}"
echo -e "${BLUE}Output: $OUTPUT_FILE${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

# =============================================================================
# Chạy decoder - GIỐNG LỆNH CỦA BẠN
# =============================================================================
"$DECODER_PATH" \
    --frameCount=1 \
    --compressedStreamPath="$BIN_FILE" \
    --reconstructedDataPath="$OUTPUT_FILE" \
    --colorSpaceConversionPath="$HDRCONVERT_PATH" \
    --inverseColorSpaceConversionConfig="$TMC2_PATH/cfg/hdrconvert/yuv420torgb444.cfg"

EXIT_CODE=$?

echo ""
echo -e "${GREEN}========================================${NC}"

# =============================================================================
# Kiểm tra kết quả
# =============================================================================
if [ $EXIT_CODE -eq 0 ]; then
    echo -e "${GREEN}[SUCCESS] Decode completed successfully!${NC}"
    
    # Đếm số file .ply được tạo
    PLY_COUNT=$(find "$DECODED_OUTPUT_DIR" -name "segment_$(printf '%03d' $SEGMENT_NUM)_${QUALITY_NAME}_${TIMESTAMP}_*.ply" 2>/dev/null | wc -l)
    
    if [ $PLY_COUNT -gt 0 ]; then
        echo -e "${GREEN}[INFO] Generated $PLY_COUNT PLY file(s)${NC}"
        
        # Hiển thị thông tin các file
        find "$DECODED_OUTPUT_DIR" -name "segment_$(printf '%03d' $SEGMENT_NUM)_${QUALITY_NAME}_${TIMESTAMP}_*.ply" 2>/dev/null | while read ply_file; do
            SIZE=$(stat -f%z "$ply_file" 2>/dev/null || stat -c%s "$ply_file" 2>/dev/null)
            SIZE_MB=$(echo "scale=2; $SIZE / 1024 / 1024" | bc)
            echo -e "${GREEN}  - $(basename $ply_file) (${SIZE_MB} MB)${NC}"
        done
    else
        echo -e "${YELLOW}[WARNING] No PLY files found in output directory${NC}"
    fi
else
    echo -e "${RED}[ERROR] Decode failed with exit code: $EXIT_CODE${NC}"
    exit $EXIT_CODE
fi

echo -e "${GREEN}========================================${NC}"

exit 0