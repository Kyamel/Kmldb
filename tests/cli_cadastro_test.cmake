string(RANDOM LENGTH 8 ALPHABET 0123456789abcdef suffix)
set(test_dir "${TEST_ROOT}/cli_cadastro_${suffix}")
file(MAKE_DIRECTORY "${test_dir}")
file(WRITE "${test_dir}/input.txt" "2\n1\nAna\n12345678900\nana@exemplo.com\n11999999999\n1990-01-01\nsegredo\nabc\n3500,75\n2\n1\n3\n1\nAna Nova\n12345678900\nana@exemplo.com\n11999999999\n1990-01-01\nnova-senha\n4200.50\n2\n1\n9\n9\n")

execute_process(
    COMMAND "${PROGRAM}"
    INPUT_FILE "${test_dir}/input.txt"
    WORKING_DIRECTORY "${test_dir}"
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
    RESULT_VARIABLE result
    TIMEOUT 10)

if(NOT result EQUAL 0)
    message(FATAL_ERROR "CLI encerrou com ${result}: ${error}")
endif()
foreach(expected IN ITEMS
    "Salário inválido"
    "Salario: 3500.75"
    "password: segredo"
    "Nome: Ana Nova"
    "Salario: 4200.50"
    "password: nova-senha")
    string(FIND "${output}" "${expected}" position)
    if(position EQUAL -1)
        message(FATAL_ERROR "Saída não contém '${expected}'")
    endif()
endforeach()
