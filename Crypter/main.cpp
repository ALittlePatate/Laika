#include <Windows.h>
#include <stdio.h>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <inttypes.h>

#include <Zydis/Zydis.h>
#include <Zycore/LibC.h>
#include <Zycore/API/Memory.h>

#include "random.hpp"
#include "utils.hpp"
#include "config.hpp"

extern g_random random;

unsigned __int64 origSeed;
int main(int argc, char* argv[]) {
    if (argc < 5)
    {
        printf("Usage: %s -i <in_file> -o <out_file> [-s <seed>] [-c <config_file>]", argv[0]);
        return 1;
    }

    unsigned __int64 genSeed = random.random_seed();

    char* arg_in = 0;
    char* arg_out = 0;
    char* arg_seed = 0;
    char* arg_config = 0;

    //==========================================================
    //    parse commandline
    //==========================================================
    for (int arg_i = 0; arg_i < argc; arg_i++)
    {
        if (strcmp(argv[arg_i], "-i") == 0)  arg_in = argv[arg_i + 1];
        if (strcmp(argv[arg_i], "-o") == 0) arg_out = argv[arg_i + 1];
        if (strcmp(argv[arg_i], "-s") == 0) arg_seed = argv[arg_i + 1];
        if (strcmp(argv[arg_i], "-c") == 0) arg_config = argv[arg_i + 1];
    };

    if (arg_in == 0 || arg_out == 0) {
        printf("Usage: %s -i <in_file> -o <out_file> [-s <seed>] [-c <config_file>]", argv[0]);
        return 1;
    }

    printf("\n IN : %s", arg_in);
    printf("\n OUT: %s", arg_out);
    printf("\n CFG: %s", arg_config);
    if (arg_seed == 0) {
        printf("\n DNA: 0x%I64X \n", genSeed);
    }
    else {
        printf("\n DNA: 0x%s", arg_seed);
    }

    if (arg_seed)
    {
        unsigned __int64 s = _strtoui64(arg_seed, NULL, 10);
        if (s == 0) {
            printf("\nBad seed format\n");
            return 1;
        }

        random.set_Seed(s);
    }
    else {
        random.set_Seed(genSeed);
    }

    origSeed = random.get_Seed();
    
    StringReplace(arg_out, ".exe", "");
    size_t n = strlen(arg_out);
    
    char* arg_out_full = (char*)malloc(n + 20); // allocating memory for the modified string
    memcpy(arg_out_full, arg_out, n);
    sprintf(&arg_out_full[n], "_0x000%I64X.exe", origSeed);

    printf("\n OUT: %s \n", arg_out_full);
    
    // check absolute path, if no convert
    if (arg_out_full && arg_out_full[1] != ':')
    {
        char* path = (char*)malloc(MAX_PATH * sizeof(char*));

        DWORD length = GetCurrentDirectoryA(MAX_PATH, path);
        if (arg_out_full[0] != '/' && arg_out_full[0] != '\\')
        {
            strcat(path, "\\");
        }

        strcat(path, arg_out_full);

        arg_out_full = path;
    }


    printf(" Input:  %s\n", arg_in);
    printf(" Output: %s\n", arg_out_full);

    if (arg_config) {
        printf(" Config: %s\n", arg_config);
    }
        

    printf(" Seed: 0x%0.8X%0.8X\n", (DWORD)(random.get_Seed() >> 32), (DWORD)random.get_Seed());

    DeleteFileA(arg_out_full);
    free(arg_out_full);

    HANDLE file_handle = CreateFileA(arg_in, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (file_handle == INVALID_HANDLE_VALUE) {
        printf(" Failed to open file %s\n", arg_in);
        return 1;
    }

    HANDLE file_mapping_handle = CreateFileMappingA(file_handle, NULL, PAGE_READONLY, 0, 0, NULL);
    if (file_mapping_handle == NULL) {
        printf(" Failed to create file mapping for %s\n", arg_in);
        CloseHandle(file_handle);
        return 1;
    }

    void* mapped_file = MapViewOfFile(file_mapping_handle, FILE_MAP_READ, 0, 0, 0);
    if (mapped_file == NULL) {
        printf(" Failed to map view of file %s\n", arg_in);
        CloseHandle(file_mapping_handle);
        CloseHandle(file_handle);
        return 1;
    }

    PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)mapped_file;
    if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
        printf(" Invalid DOS signature in %s\n", arg_in);
        UnmapViewOfFile(mapped_file);
        CloseHandle(file_mapping_handle);
        CloseHandle(file_handle);
        return 1;
    }

    PIMAGE_NT_HEADERS nt_headers = (PIMAGE_NT_HEADERS)((BYTE*)mapped_file + dos_header->e_lfanew);
    if (nt_headers->Signature != IMAGE_NT_SIGNATURE) {
        printf(" Invalid NT signature in %s\n", arg_in);
        UnmapViewOfFile(mapped_file);
        CloseHandle(file_mapping_handle);
        CloseHandle(file_handle);
        return 1;
    }

    DWORD text_addr = 0x0;
    DWORD text_size = 0x0;
    PIMAGE_SECTION_HEADER section_headers = (PIMAGE_SECTION_HEADER)((BYTE*)&nt_headers->OptionalHeader + nt_headers->FileHeader.SizeOfOptionalHeader);
    for (WORD i = 0; i < nt_headers->FileHeader.NumberOfSections; ++i) {
        if (strncmp((char*)section_headers[i].Name, ".text", 5) == 0) {
            text_addr = section_headers[i].PointerToRawData;
            text_size = section_headers[i].SizeOfRawData;
            break;
        }
    }

    printf("\n");
    printf(" Enumerating .text instructions and finding the stuff to change...\n");

    // Initialize decoder context
    ZydisDecoder decoder;
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LEGACY_32, ZYDIS_STACK_WIDTH_32);

    // Initialize formatter. Only required when you actually plan to do instruction
    // formatting ("disassembling"), like we do here
    ZydisFormatter formatter;
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);
    
    // Loop over the instructions in our buffer.
    // The runtime-address (instruction pointer) is chosen arbitrary here in order to better
    // visualize relative addressing
    ZyanU64 runtime_address = nt_headers->OptionalHeader.ImageBase - 0x1000;
    ZyanUSize offset = text_addr;
    const ZyanUSize length = sizeof((char*)mapped_file);
    ZydisDecodedInstruction instruction;
    ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];

    while (ZYAN_SUCCESS(ZydisDecoderDecodeFull(&decoder, (char*)mapped_file + offset, length - offset,
        &instruction, operands)) && offset < text_size + text_addr)
    {
        // Print current instruction pointer.
        printf(" %016" PRIX64 "  ", runtime_address);

        // Format & print the binary instruction structure to human-readable format
        char buffer[256];
        ZydisFormatterFormatInstruction(&formatter, &instruction, operands, instruction.operand_count_visible, buffer, sizeof(buffer), runtime_address, ZYAN_NULL);
        puts(buffer);

        // Create an encoder request from the previously decoded instruction.
        ZydisEncoderRequest enc_req;
        ZydisEncoderDecodedInstructionToEncoderRequest(&instruction, operands,
            instruction.operand_count_visible, &enc_req);

        // Now, change some things about the instruction!

        // Change `jz` -> `jnz` and `add` -> `sub`.
        bool changed = true;
        switch (enc_req.mnemonic)
        {
        case ZYDIS_MNEMONIC_ADD:
            enc_req.mnemonic = ZYDIS_MNEMONIC_SUB;
            break;
        case ZYDIS_MNEMONIC_JZ:
            enc_req.mnemonic = ZYDIS_MNEMONIC_JNZ;
            break;
        default:
            // Don't change other instructions.
            changed = false;
            break;
        }

        if (changed) {
            printf(" Instruction %s changed\n", buffer);
        }

        // Encode the instruction back to raw bytes.
        uint8_t new_bytes[ZYDIS_MAX_INSTRUCTION_LENGTH];
        ZyanUSize new_instr_length = sizeof(new_bytes);
        ZydisEncoderEncodeInstruction(&enc_req, new_bytes, &new_instr_length);

        // Decode and print the new instruction. We re-use the old buffers.
        ZydisDecoderDecodeFull(&decoder, new_bytes, new_instr_length, &instruction,
            operands);
        ZydisFormatterFormatInstruction(&formatter, &instruction, operands,
            instruction.operand_count_visible, buffer, sizeof(buffer), 0, NULL);
        printf(" New instruction:      %s\n", buffer);

        offset += instruction.length;
        runtime_address += instruction.length;
    }

    UnmapViewOfFile(mapped_file);
    CloseHandle(file_mapping_handle);
    CloseHandle(file_handle);

    return 0;
}