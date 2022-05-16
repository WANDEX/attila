#include "tgrp.h"

int main(int argc, char* argv[])
{
    std::string file_path = "../week-01-2022.txt";
    std::vector<Task> tasks = file_tasks(file_path);
    return 0;
}
