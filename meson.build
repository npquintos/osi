project('OsiDB', 'cpp',
    default_options : ['cpp_std=c++20']
  )
st = executable('split_test',
    'src/test_split.cpp',
    '../../include/split.hpp',
    '../../include/doctest.h'
  )
rt = executable('linereader_test',
    'src/test_linereader.cpp',
    '../../include/qreader.hpp',
    '../../include/doctest.h'
    )
dt = executable('db_test',
    'src/test_db.cpp',
    '../../include/osiDb.hpp',
    '../../include/doctest.h'
    )
test('Qstring Tests', st)
test('Read Line Tests', rt)
test('OSI DB Tests', dt)
