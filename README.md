# Try Again

https://github.com/user-attachments/assets/c76ab41e-e336-4370-b43d-d01c229d2dae

This project is a simple command-line tool to play back fictional conversations stored in a custom `.cnv` file format written in raw C.  
Originally wrote it because I was bored, so some bugs may be. Named it "Try Again" because that's the first stories name which inspired me to create that project.  

## How to Use

1.  **Compile `main.c`:** Use a C compiler like GCC to compile the `main.c` file. For example:

    ```bash
    gcc main.c -o main.exe
    ```

2.  **Run the program:** Execute the compiled program. By default, it will play the `try-again.cnv` file.

    ```bash
    ./main.exe
    ```

    You can also specify a different `.cnv` file as a command-line argument:

    ```bash
    ./main.exe ./conversation/your-conversation.cnv
    ```

    Many cute stories (everything LLM-generated except Try Again) can be found in the `conversation` folder. Note: You have to press enter to continue to the next message.

## `.cnv` File Format

The `.cnv` file format is a binary format that stores the conversation data. It is structured as follows:

1.  **Notices:** A `uint8_t` representing the number of notices, followed by the notices themselves. Each notice is a `uint8_t` for the length, followed by the notice string.
2.  **Names:** Two names, each with a `uint8_t` for the length, followed by the name string.
3.  **Messages:** A `uint16_t` for the number of messages, followed by the messages. Each message has a `uint8_t` for the length, the message string, and a `uint16_t` for the timing.
