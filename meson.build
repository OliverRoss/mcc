project('mcc', 'c',
    default_options: [ 'buildtype=release',
                       'c_std=c11',
                       'warning_level=3' ],
    meson_version: '>=0.47.0'
)

# ------------------------------------------------------------------ Generators

flex = find_program('flex')
lgen = generator(flex,
                 output: ['@BASENAME@.c', '@BASENAME@.h'],
                 arguments: [ '--outfile=@OUTPUT0@',
                              '--header-file=@OUTPUT1@',
                              '@INPUT@' ])

bison = find_program('bison')
pgen = generator(bison,
                 output: ['@BASENAME@.tab.c', '@BASENAME@.tab.h'],
                 arguments: [ '-Wall',
                              '--output=@OUTPUT0@',
                              '--defines=@OUTPUT1@',
                              '@INPUT@' ])


# --------------------------------------------------------------------- Library

mcc_inc = include_directories('include')

mcc_src = [ 'src/utils/print_string.c' ,
            'src/utils/length_of_int.c',
            'src/ast.c',
            'src/ast_print.c',
            'src/ast_visit.c',
            'src/semantic_checks.c',
            'src/ir.c',
            'src/ir_print.c',
            'src/cfg.c',
            'src/cfg_print.c',
            'src/asm.c',
            'src/asm_print.c',
            'src/stack_size.c',
            lgen.process('src/scanner.l'),
            pgen.process('src/parser.y'),
            'src/symbol_table.c',
            'src/symbol_table_print.c',
            'src/symbol_table_print_dot.c']

mcc_lib = library('mcc', mcc_src,
                  c_args: '-D_POSIX_C_SOURCE=200809L',
                  link_args: '-lm',
                  include_directories: [mcc_inc, include_directories('src')])

mc_builtins = configure_file(input : 'resources/mc_builtins.c',
                             output : 'mc_builtins.c',
                             copy : true)

# ---------------------------------------------------------------- Applications

mcc_apps = [ 'mcc', 'mc_ast_to_dot','mc_symbol_table','mc_ir','mc_cfg_to_dot','mc_asm']

foreach app : mcc_apps
    executable(app, 'app/' + app + '.c',
               include_directories: mcc_inc,
               link_with: mcc_lib)
endforeach

# ----------------------------------------------------------------------- Tests

mcc_tests = [ 'parser_test', 'symbol_table_test', 'semantic_checks_test','ir_test', 'asm_test', 'stack_size_test']

cutest_inc = include_directories('vendor/cutest')

foreach test : mcc_tests
    t = executable(test, 'test/unit/' + test + '.c', 'vendor/cutest/CuTest.c',
                   include_directories: [mcc_inc, cutest_inc],
                   link_with: mcc_lib)
    test(test, t)
endforeach
