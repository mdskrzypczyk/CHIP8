#include "chip8.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"


class MockVIDEO : public VIDEO {
  public:
    MOCK_METHOD(bool, init, ());
    MOCK_METHOD(void, show, ());
    MOCK_METHOD(void, clear, ());
    MOCK_METHOD(void, close, ());
    MOCK_METHOD(uint32_t**, get_pix_map, ());
    MOCK_METHOD(void, draw_pix_map, ());
    MOCK_METHOD(void, handle_event, (SDL_Event event));
    MOCK_METHOD(void, rand_color_scheme, ());
    MOCK_METHOD(bool, xor_color, (uint8_t x, uint8_t y));
};

class MockAUDIO : public AUDIO {
  public:
    MOCK_METHOD(bool, init, ());
    MOCK_METHOD(void, play_tone, ());
};

class MockINPUT : public INPUT {
  public:
    MOCK_METHOD(uint8_t, poll_keyboard, (SDL_Event event));
    MOCK_METHOD(bool, get_key_status, (uint8_t key));
};

TEST(CHIP8Tests, TestConstructor) {
    CHIP8 chip8 = CHIP8();

    EXPECT_EQ(chip8.get_quit(), false);
    EXPECT_EQ(chip8.get_draw(), true);
    EXPECT_EQ(chip8.get_pc(), PC_START);
    EXPECT_EQ(chip8.get_sp(), 0xFF);
    EXPECT_EQ(chip8.get_index_reg(), 0);
    EXPECT_EQ(chip8.get_delay_timer(), 0);
    EXPECT_EQ(chip8.get_sound_timer(), 0);

    uint16_t *STACK = chip8.get_stack();
    for (int i = 0; i < STACK_SIZE; i++) {
        EXPECT_EQ(STACK[i], 0);
    }

    uint8_t *V = chip8.get_reg_file();
    for (int i = 0; i < REG_SIZE; i++) {
        EXPECT_EQ(V[i], 0);
    }

    uint8_t *MEM = chip8.get_mem();
    for (int i = 0; i < MEM_SIZE; i++) {
        if (i < MAP_LENGTH) {
            EXPECT_EQ(MEM[i], SPRITE_MAP[i]);
        } else {
            EXPECT_EQ(MEM[i], 0);
        }
    }
}

TEST(CHIP8Tests, DISABLED_TestInitComponents) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);
}

TEST(CHIP8Tests, TestLoadProgram) {
    CHIP8 chip8 = CHIP8();
    char bad_path[] = "";
    EXPECT_EQ(chip8.load_program(bad_path), false);

    char test_rom_path[] = "test_opcode.ch8";
    EXPECT_EQ(chip8.load_program(test_rom_path), true);

    uint8_t program_data[MAX_PROG_SIZE];
    for (int i = 0; i < MAX_PROG_SIZE; i++) {
        program_data[i] = 0x00;
    }

    // Open chip 8 game
    FILE *program_file = fopen(test_rom_path, "r+");
    EXPECT_NE(program_file, nullptr);

    // Obtain the file size
    fseek(program_file, 0, SEEK_END);
    size_t f_size = ftell(program_file);
    rewind(program_file);

    size_t result =
            fread(program_data, sizeof(uint8_t), MAX_PROG_SIZE, program_file);
    EXPECT_EQ(result, f_size);

    fclose(program_file);

    // Copy from the data array into memory, this can be skipped by having fread
    // read directly into the correct starting address of memory
    uint8_t *MEM = chip8.get_mem();
    for (int i = 0; i < MAX_PROG_SIZE; i++) {
        EXPECT_EQ(MEM[PC_START + i], program_data[i]);
    }
}

TEST(CHIP8Tests, DISABLED_TestSaveState) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    char test_rom_path[] = "test_opcode.ch8";
    EXPECT_EQ(chip8.load_program(test_rom_path), true);

    const char state_path[] = "test_save.sav";
    EXPECT_EQ(chip8.save_state(state_path), true);
    EXPECT_EQ(chip8.load_state(state_path), true);
}

TEST(CHIP8Tests, DISABLED_DISABLED_TestExecOp_00E0) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x00E0;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(true, false);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_00EE) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x00EE;
    uint16_t* stack = chip8.get_stack();
    uint8_t curr_sp = chip8.get_sp();
    uint16_t curr_pc = chip8.get_pc();

    chip8.exec_op(test_opcode);

    EXPECT_EQ(stack[curr_sp], chip8.get_pc());

    EXPECT_EQ(chip8.get_sp(), curr_sp - 1);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_1nnn) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x1000;
    uint16_t test_nibble = 0x0123;
    test_opcode = test_opcode | test_nibble;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(chip8.get_pc(), test_nibble);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_2nnn) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x2000;
    uint16_t test_nibble = 0x0123;
    test_opcode = test_opcode | test_nibble;

    uint8_t curr_sp = chip8.get_sp();
    uint16_t curr_pc = chip8.get_pc();
    uint16_t *stack = chip8.get_stack();

    chip8.exec_op(test_opcode);

    EXPECT_EQ(chip8.get_sp(), (uint8_t)(curr_sp + 1));
    EXPECT_EQ(stack[chip8.get_sp()], curr_pc + 2);
    EXPECT_EQ(chip8.get_pc(), test_nibble);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_3xkk) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x3000;
    uint16_t x = 0x1;
    uint16_t kk = 0x23;
    test_opcode = test_opcode | (x << 8) | kk;

    uint16_t curr_pc = chip8.get_pc();
    uint8_t *v = chip8.get_reg_file();

    v[x] = 0x00;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(chip8.get_pc(), curr_pc + 2);

    v[x] = kk;
    curr_pc = chip8.get_pc();
    chip8.exec_op(test_opcode);
    EXPECT_EQ(chip8.get_pc(), curr_pc + 4);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_4xkk) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x4000;
    uint16_t x = 0x1;
    uint16_t kk = 0x23;
    test_opcode = test_opcode | (x << 8) | kk;

    uint16_t curr_pc = chip8.get_pc();
    uint8_t *v = chip8.get_reg_file();

    v[x] = 0x00;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(chip8.get_pc(), curr_pc + 4);

    v[x] = kk;
    curr_pc = chip8.get_pc();
    chip8.exec_op(test_opcode);
    EXPECT_EQ(chip8.get_pc(), curr_pc + 2);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_5xy0) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x5000;
    uint16_t x = 0x1;
    uint16_t y = 0x2;
    test_opcode = test_opcode | (x << 8) | (y << 4);

    uint16_t curr_pc = chip8.get_pc();
    uint8_t *v = chip8.get_reg_file();

    v[x] = v[y];
    chip8.exec_op(test_opcode);
    EXPECT_EQ(chip8.get_pc(), curr_pc + 4);

    v[x] = !v[y];
    curr_pc = chip8.get_pc();
    chip8.exec_op(test_opcode);
    EXPECT_EQ(chip8.get_pc(), curr_pc + 2);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_6xkk) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x6000;
    uint16_t x = 0x1;
    uint16_t kk = 0x23;
    test_opcode = test_opcode | (x << 8) | kk;

    uint16_t curr_pc = chip8.get_pc();
    uint8_t *v = chip8.get_reg_file();

    chip8.exec_op(test_opcode);
    EXPECT_EQ(v[x], kk);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_7xkk) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x7000;
    uint16_t x = 0x1;
    uint16_t kk = 0x23;
    test_opcode = test_opcode | (x << 8) | kk;

    uint16_t curr_pc = chip8.get_pc();
    uint8_t *v = chip8.get_reg_file();

    chip8.exec_op(test_opcode);
    EXPECT_EQ(v[x], kk);

    chip8.exec_op(test_opcode);
    EXPECT_EQ(v[x], 2*kk);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_8xy0) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x8000;
    uint16_t x = 0x1;
    uint16_t y = 0x2;
    uint8_t val = 0x12;
    test_opcode = test_opcode | (x << 8) | (y << 4);

    uint8_t *v = chip8.get_reg_file();

    v[x] = val;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(v[x], v[y]);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_8xy1) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x8001;
    uint16_t x = 0x1;
    uint16_t y = 0x2;
    uint8_t valx = 0x11;
    uint8_t valy = 0x88;
    test_opcode = test_opcode | (x << 8) | (y << 4);

    uint8_t *v = chip8.get_reg_file();

    v[x] = valx;
    v[y] = valy;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(v[x], valx | valy);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_8xy2) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x8002;
    uint16_t x = 0x1;
    uint16_t y = 0x2;
    uint8_t valx = 0x11;
    uint8_t valy = 0x88;
    test_opcode = test_opcode | (x << 8) | (y << 4);

    uint8_t *v = chip8.get_reg_file();

    v[x] = valx;
    v[y] = valy;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(v[x], valx & valy);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_8xy3) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x8003;
    uint16_t x = 0x1;
    uint16_t y = 0x2;
    uint8_t valx = 0x11;
    uint8_t valy = 0x88;
    test_opcode = test_opcode | (x << 8) | (y << 4);

    uint8_t *v = chip8.get_reg_file();

    v[x] = valx;
    v[y] = valy;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(v[x], valx ^ valy);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_8xy4) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x8004;
    uint16_t x = 0x1;
    uint16_t y = 0x2;
    uint8_t valx = 0xFF;
    uint8_t valy = 0x01;
    test_opcode = test_opcode | (x << 8) | (y << 4);

    uint8_t *v = chip8.get_reg_file();

    v[x] = valx;
    v[y] = valy;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(v[0xF], 1);
    EXPECT_EQ(v[x], (uint8_t) (valx + valy));

    valx = 0x01;
    v[x] = valx;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(v[0xF], 0);
    EXPECT_EQ(v[x], (uint8_t) (valx + valy));
}

TEST(CHIP8Tests, DISABLED_TestExecOp_8xy5) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x8005;
    uint16_t x = 0x1;
    uint16_t y = 0x2;
    uint8_t valx = 0xFF;
    uint8_t valy = 0x01;
    test_opcode = test_opcode | (x << 8) | (y << 4);

    uint8_t *v = chip8.get_reg_file();

    v[x] = valx;
    v[y] = valy;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(v[0xF], 1);
    EXPECT_EQ(v[x], (uint8_t) (valx - valy));

    valx = 0x01;
    v[x] = valx;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(v[0xF], 0);
    EXPECT_EQ(v[x], (uint8_t) (valx - valy));
}

TEST(CHIP8Tests, DISABLED_TestExecOp_8xy6) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x8006;
    uint16_t x = 0x1;
    uint16_t y = 0x2;
    uint8_t valx = 0x1;
    test_opcode = test_opcode | (x << 8) | (y << 4);

    uint8_t *v = chip8.get_reg_file();

    v[x] = valx;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(v[0xF], 1);
    EXPECT_EQ(v[x], (uint8_t) (valx >> 1));

    valx = 0x8;
    v[x] = valx;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(v[0xF], 0);
    EXPECT_EQ(v[x], (uint8_t) (valx >> 1));
}

TEST(CHIP8Tests, DISABLED_TestExecOp_8xy7) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x8007;
    uint16_t x = 0x1;
    uint16_t y = 0x2;
    uint8_t valx = 0x1;
    test_opcode = test_opcode | (x << 8) | (y << 4);

    uint8_t *v = chip8.get_reg_file();

    v[x] = valx;
    v[y] = v[x] + 1;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(v[0xF], 1);
    EXPECT_EQ(v[x], 1);

    v[y] = v[x] - 1;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(v[0xF], 0);
    EXPECT_EQ(v[x], 0xFF);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_8xyE) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x800E;
    uint16_t x = 0x1;
    uint16_t y = 0x2;
    uint8_t valx = 0x80;
    test_opcode = test_opcode | (x << 8) | (y << 4);

    uint8_t *v = chip8.get_reg_file();

    v[x] = valx;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(v[0xF], 1);
    EXPECT_EQ(v[x], 0x0);

    valx = 0x70;
    v[x] = valx;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(v[0xF], 0);
    EXPECT_EQ(v[x], 0xE0);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_9xy0) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0x9000;
    uint16_t x = 0x1;
    uint16_t y = 0x2;
    uint8_t valx = 0x80;
    uint8_t valy = 0x01;
    test_opcode = test_opcode | (x << 8) | (y << 4);

    uint8_t *v = chip8.get_reg_file();
    uint16_t curr_pc = chip8.get_pc();

    v[x] = valx;
    v[y] = valx;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(chip8.get_pc(), curr_pc + 2);

    v[y] = valy;
    curr_pc = chip8.get_pc();
    chip8.exec_op(test_opcode);
    EXPECT_EQ(chip8.get_pc(), curr_pc + 4);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_Annn) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0xA000;
    uint16_t nnn = 0x0123;
    test_opcode = test_opcode | nnn;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(chip8.get_index_reg(), nnn);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_Bnnn) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0xB000;
    uint16_t nnn = 0x0123;
    uint8_t val0 = 0x6;

    uint8_t *v = chip8.get_reg_file();
    v[0] = val0;

    test_opcode = test_opcode | nnn;
    chip8.exec_op(test_opcode);
    EXPECT_EQ(chip8.get_pc(), nnn + val0);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_Cxkk) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0xC000;
    uint16_t x = 0x0001;
    uint8_t kk = 0xFF;

    uint8_t *v = chip8.get_reg_file();

    test_opcode = test_opcode | (x << 8) | kk;
    chip8.exec_op(test_opcode);
    EXPECT_NE(v[x], 0x00);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_Ex9E) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0xE09E;
    uint16_t x = 0x0001;
    uint8_t valx = 0x0;

    test_opcode = test_opcode | (x << 8);

    uint16_t curr_pc = chip8.get_pc();
    uint8_t *v = chip8.get_reg_file();
    v[x] = valx;

    INPUT *input = chip8.get_input_device();
    EXPECT_EQ(input->get_key_status(valx), false);

    chip8.exec_op(test_opcode);
    EXPECT_EQ(chip8.get_pc(), curr_pc + 2);

    input->flip_key_status(valx);
    EXPECT_EQ(input->get_key_status(valx), true);

    curr_pc = chip8.get_pc();
    chip8.exec_op(test_opcode);
    EXPECT_EQ(chip8.get_pc(), curr_pc + 4);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_ExA1) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0xE0A1;
    uint16_t x = 0x0001;
    uint8_t valx = 0x0;

    test_opcode = test_opcode | (x << 8);

    uint16_t curr_pc = chip8.get_pc();
    uint8_t *v = chip8.get_reg_file();
    v[x] = valx;

    INPUT *input = chip8.get_input_device();
    EXPECT_EQ(input->get_key_status(valx), false);

    chip8.exec_op(test_opcode);
    EXPECT_EQ(chip8.get_pc(), curr_pc + 4);

    input->flip_key_status(valx);
    EXPECT_EQ(input->get_key_status(valx), true);

    curr_pc = chip8.get_pc();
    chip8.exec_op(test_opcode);
    EXPECT_EQ(chip8.get_pc(), curr_pc + 2);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_Fx07) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0xF007;
    uint16_t x = 0x0001;

    test_opcode = test_opcode | (x << 8);

    uint16_t curr_pc = chip8.get_pc();
    uint8_t *v = chip8.get_reg_file();
    uint16_t curr_dt = chip8.get_delay_timer();

    chip8.exec_op(test_opcode);
    EXPECT_EQ(v[x], curr_dt);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_Fx15) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0xF015;
    uint16_t x = 0x0001;
    uint8_t valx = 0x23;
    uint8_t *v = chip8.get_reg_file();
    v[x] = valx;

    test_opcode = test_opcode | (x << 8);

    chip8.exec_op(test_opcode);
    uint16_t curr_dt = chip8.get_delay_timer();

    EXPECT_EQ(valx, curr_dt);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_Fx18) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0xF018;
    uint16_t x = 0x0001;
    uint8_t valx = 0x23;
    uint8_t *v = chip8.get_reg_file();
    v[x] = valx;

    test_opcode = test_opcode | (x << 8);

    chip8.exec_op(test_opcode);
    uint16_t curr_st = chip8.get_sound_timer();

    EXPECT_EQ(valx, curr_st);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_Fx1E) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0xF01E;
    uint16_t x = 0x0001;
    uint8_t valx = 0x23;
    uint8_t *v = chip8.get_reg_file();
    v[x] = valx;

    test_opcode = test_opcode | (x << 8);

    uint16_t curr_index_reg = chip8.get_index_reg();
    chip8.exec_op(test_opcode);

    EXPECT_EQ(chip8.get_index_reg(), curr_index_reg + valx);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_Fx29) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0xF029;
    uint16_t x = 0x0001;
    uint8_t valx = 0x23;
    uint8_t *v = chip8.get_reg_file();
    v[x] = valx;

    test_opcode = test_opcode | (x << 8);

    uint16_t curr_index_reg = chip8.get_index_reg();
    chip8.exec_op(test_opcode);

    EXPECT_EQ(chip8.get_index_reg(), (uint16_t) (5*valx));
}

TEST(CHIP8Tests, DISABLED_TestExecOp_Fx33) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0xF033;
    uint16_t x = 0x0001;
    uint8_t valx = 123;
    uint8_t *v = chip8.get_reg_file();
    v[x] = valx;

    test_opcode = test_opcode | (x << 8);

    uint16_t curr_index_reg = chip8.get_index_reg();
    uint8_t *curr_mem = chip8.get_mem();
    chip8.exec_op(test_opcode);
    EXPECT_EQ(curr_mem[curr_index_reg], 1);
    EXPECT_EQ(curr_mem[curr_index_reg + 1], 2);
    EXPECT_EQ(curr_mem[curr_index_reg + 2], 3);
}

TEST(CHIP8Tests, DISABLED_TestExecOp_Fx55) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0xFF55;
    uint8_t *v = chip8.get_reg_file();
    for (uint8_t i = 0; i < REG_SIZE; i++) {
        v[i] = i;
    }

    uint16_t curr_index_reg = chip8.get_index_reg();
    uint8_t *curr_mem = chip8.get_mem();
    chip8.exec_op(test_opcode);

    for (uint8_t i = 0; i < REG_SIZE; i++) {
        curr_mem[curr_index_reg + i] = i;
    }
}

TEST(CHIP8Tests, DISABLED_TestExecOp_Fx65) {
    CHIP8 chip8 = CHIP8();
    EXPECT_EQ(chip8.init_video(), true);
    EXPECT_EQ(chip8.init_audio(), true);

    uint16_t test_opcode = 0xFF65;

    uint16_t curr_index_reg = chip8.get_index_reg();
    uint8_t *curr_mem = chip8.get_mem();

    for (uint8_t i = 0; i < REG_SIZE; i++) {
        curr_mem[curr_index_reg + i] = i;
    }

    chip8.exec_op(test_opcode);

    uint8_t *v = chip8.get_reg_file();
    for (uint8_t i = 0; i < REG_SIZE; i++) {
        EXPECT_EQ(v[i], i);
    }
}

TEST(CHIP8Tests, TestPrintMemContents) {
    CHIP8 chip8 = CHIP8();
    chip8.print_mem_contents();
}

TEST(CHIP8Tests, TestPrintSysContents) {
    CHIP8 chip8 = CHIP8();
    chip8.print_sys_contents();
}