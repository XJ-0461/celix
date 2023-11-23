/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "gtest/gtest.h"

extern "C" {
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <ffi.h>

#include "dyn_type_common.h"
#include "dyn_common.h"
#include "dyn_type.h"
#include "json_serializer.h"
#include "celix_err.h"

static void stdLog(void*, int level, const char *file, int line, const char *msg, ...) {
	va_list ap;
	const char *levels[5] = {"NIL", "ERROR", "WARNING", "INFO", "DEBUG"};
	fprintf(stderr, "%s: FILE:%s, LINE:%i, MSG:",levels[level], file, line);
	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\n");
	va_end(ap);
}

/*********** example 1 ************************/
/** struct type ******************************/
const char *example1_descriptor = "{DJISF a b c d e}";

static const char *avpr_example1_descriptor = "{\
                \"protocol\" : \"types\", \
                \"namespace\" : \"test.ns\", \
                \"version\" : \"1.0.0\", \
                \"types\" : [ { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Double\", \
                    \"size\" : 8, \
                    \"alias\" : \"double\" \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Slong\", \
                    \"size\" : 8, \
                    \"signed\" : true \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Sint\", \
                    \"size\" : 4, \
                    \"signed\" : true \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Short\", \
                    \"size\" : 2, \
                    \"signed\" : true \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Float\", \
                    \"size\" : 4, \
                    \"alias\" : \"float\" \
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"structA\", \
                    \"fields\" : [{\
                        \"name\" : \"a\", \
                        \"type\" : \"Double\" \
                    }, {\
                        \"name\" : \"b\", \
                        \"type\" : \"Slong\" \
                    }, {\
                        \"name\" : \"c\", \
                        \"type\" : \"Sint\" \
                    }, {\
                        \"name\" : \"d\", \
                        \"type\" : \"Short\" \
                    }, {\
                        \"name\" : \"e\", \
                        \"type\" : \"Float\" \
                    }]\
                }], \
                \"messages\" : {} \
            }";

static const char *avpr_example1_fqn = "test.ns.structA";

const char *example1_input = "{ \
    \"a\" : 1.0, \
    \"b\" : 22, \
    \"c\" : 32, \
    \"d\" : 42, \
    \"e\" : 4.4 \
}";

struct example1 {
	double a;   //0
	int64_t b;  //1
	int32_t c;  //2
	int16_t d;  //3
	float e;    //4
};

static void check_example1(void *data) {
	auto ex = static_cast<example1*>(data);
	ASSERT_EQ(1.0, ex->a);
	ASSERT_EQ(22, ex->b);
	ASSERT_EQ(32, ex->c);
	ASSERT_EQ(42, ex->d);
	ASSERT_EQ(4.4f, ex->e);
}

/*********** example 2 ************************/
const char *example2_descriptor = "{BJJDFD byte long1 long2 double1 float1 double2}";

static const char *avpr_example2_descriptor = "{\
                \"protocol\" : \"types\", \
                \"namespace\" : \"test.ns\", \
                \"version\" : \"1.0.0\", \
                \"types\" : [ { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Byte\", \
                    \"size\" : 1, \
                    \"signed\" : true \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Slong\", \
                    \"size\" : 8, \
                    \"signed\" : true \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Double\", \
                    \"size\" : 8, \
                    \"alias\" : \"double\" \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Float\", \
                    \"size\" : 4, \
                    \"alias\" : \"float\" \
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"structB\", \
                    \"fields\" : [{\
                        \"name\" : \"byte\", \
                        \"type\" : \"Byte\" \
                    }, {\
                        \"name\" : \"long1\", \
                        \"type\" : \"Slong\" \
                    }, {\
                        \"name\" : \"long2\", \
                        \"type\" : \"Slong\" \
                    }, {\
                        \"name\" : \"double1\", \
                        \"type\" : \"Double\" \
                    }, {\
                        \"name\" : \"float1\", \
                        \"type\" : \"Float\" \
                    }, {\
                        \"name\" : \"double2\", \
                        \"type\" : \"Double\" \
                    }]\
                }], \
                \"messages\" : {} \
            }";

static const char *avpr_example2_fqn = "test.ns.structB";

const char *example2_input = "{ \
    \"byte\" : 42, \
    \"long1\" : 232, \
    \"long2\" : 242, \
    \"double1\" : 4.2, \
    \"float1\" : 3.2, \
    \"double2\" : 4.4 \
}";

struct example2 {
	char byte;      //0
	int64_t long1;     //1
	int64_t long2;     //2
	double double1; //3
	float float1;   //4
	double double2; //5
};

static void check_example2(void *data) {
    auto ex = static_cast<example2*>(data);
	ASSERT_EQ(42, ex->byte);
	ASSERT_EQ(232, ex->long1);
	ASSERT_EQ(242, ex->long2);
	ASSERT_EQ(4.2, ex->double1);
	ASSERT_EQ(3.2f, ex->float1);
	ASSERT_EQ(4.4, ex->double2);
}


/*********** example 3 ************************/
/** sequence with a simple type **************/
const char *example3_descriptor = "{[I numbers}";

static const char *avpr_example3_descriptor = "{\
                \"protocol\" : \"types\", \
                \"namespace\" : \"test.ns\", \
                \"version\" : \"1.0.0\", \
                \"types\" : [ { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Sint\", \
                    \"size\" : 4, \
                    \"signed\" : true \
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"structC\", \
                    \"fields\" : [{\
                        \"name\" : \"numbers\", \
                        \"type\" : {\
                            \"type\" : \"array\", \
                            \"items\" : \"Sint\", \
                            \"static\" : 3 \
                        }\
                    }]\
                }], \
                \"messages\" : {} \
            }";

static const char *avpr_example3_fqn = "test.ns.structC";

const char *example3_input = "{ \
    \"numbers\" : [22,32,42] \
}";

struct example3 {
	struct {
		uint32_t cap;
		uint32_t len;
		int32_t *buf;
	} numbers;
};

static void check_example3(void *data) {
    auto ex = static_cast<example3*>(data);
	ASSERT_EQ(3, ex->numbers.len);
	ASSERT_EQ(22, ex->numbers.buf[0]);
	ASSERT_EQ(32, ex->numbers.buf[1]);
	ASSERT_EQ(42, ex->numbers.buf[2]);
}

/*********** example 4 ************************/
/** structs within a struct (by reference)*******/
const char *example4_descriptor = "{{IDD index val1 val2}{IDD index val1 val2} left right}";

static const char *avpr_example4_descriptor = "{\
                \"protocol\" : \"types\", \
                \"namespace\" : \"test.ns\", \
                \"version\" : \"1.0.0\", \
                \"types\" : [ { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Int32\", \
                    \"size\" : 4, \
                    \"signed\" : true \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Double\", \
                    \"size\" : 8, \
                    \"alias\" : \"double\" \
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"leaf\", \
                    \"namespace\" : \"details\", \
                    \"fields\" : [{\
                        \"name\" : \"index\", \
                        \"type\" : \"test.ns.Int32\" \
                    }, {\
                        \"name\" : \"val1\", \
                        \"type\" : \"test.ns.Double\" \
                    }, {\
                        \"name\" : \"val2\", \
                        \"type\" : \"test.ns.Double\" \
                    }]\
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"structD\", \
                    \"fields\" : [{\
                        \"name\" : \"left\", \
                        \"type\" : \"details.leaf\" \
                    }, {\
                        \"name\" : \"right\", \
                        \"type\" : \"details.leaf\" \
                    }]\
                }], \
                \"messages\" : {} \
            }";

static const char *avpr_example4_fqn = "test.ns.structD";

static const char *example4_input =  "{ \
    \"left\" : {\"index\":1, \"val1\":1.0, \"val2\":2.0 }, \
    \"right\" : {\"index\":2, \"val1\":5.0, \"val2\":4.0 } \
}";

struct ex4_leaf {
	int32_t index;
	double val1;
	double val2;
};

struct example4 {
	struct ex4_leaf left;
	struct ex4_leaf right;
};

static void check_example4(void *data) {
    auto ex = static_cast<example4*>(data);
	ASSERT_EQ(1, ex->left.index);
	ASSERT_EQ(1.0, ex->left.val1);
	ASSERT_EQ(2.0, ex->left.val2);
	ASSERT_EQ(2, ex->right.index);
	ASSERT_EQ(5.0, ex->right.val1);
	ASSERT_EQ(4.0, ex->right.val2);
}


/*********** example 5 ************************/
/** structs within a struct (by reference)*******/
const char *example5_descriptor = "Tleaf={ts name age};Tnode={Lnode;Lnode;Lleaf; left right value};{Lnode; head}";

static const char *avpr_example5_descriptor = "{\
                \"protocol\" : \"types\", \
                \"namespace\" : \"test.ns\", \
                \"version\" : \"1.0.0\", \
                \"types\" : [ { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Char\", \
                    \"size\" : 2 \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Str\", \
                    \"size\" : 8, \
                    \"alias\" : \"string\" \
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"leaf\", \
                    \"namespace\" : \"details\", \
                    \"fields\" : [{\
                        \"name\" : \"name\", \
                        \"type\" : \"test.ns.Str\" \
                    }, {\
                        \"name\" : \"age\", \
                        \"type\" : \"test.ns.Char\" \
                    }]\
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"node\", \
                    \"fields\" : [{\
                        \"name\" : \"left\", \
                        \"type\" : \"node\", \
                        \"ptr\" : true \
                    }, {\
                        \"name\" : \"right\", \
                        \"type\" : \"node\", \
                        \"ptr\" : true \
                    }, {\
                        \"name\" : \"value\", \
                        \"type\" : \"details.leaf\", \
                        \"ptr\" : true \
                    }]\
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"structE\", \
                    \"fields\" : [{\
                        \"name\" : \"head\", \
                        \"type\" : \"node\", \
                        \"ptr\" : true \
                    }]\
                }], \
                \"messages\" : {} \
            }";

static const char *avpr_example5_fqn = "test.ns.structE";

static const char *example5_input =  "{ \
    \"head\" : {\
        \"left\" : {\
            \"value\" : {\
                \"name\" : \"John\",\
                \"age\" : 44 \
            },\
            \"left\" : {\
                \"value\" : {\
                    \"name\" : \"Victor\",\
                    \"age\" : 400 \
                }\
            }\
        },\
        \"right\" : {\
            \"value\" : {\
                \"name\" : \"Peter\", \
                \"age\" : 55 \
            }\
        }\
    }\
}";

struct leaf {
	const char *name;
	uint16_t age;
};

struct node {
	struct node *left;
	struct node *right;
	struct leaf *value;
};

struct example5 {
	struct node *head;
};

static void check_example5(void *data) {
    auto ex = static_cast<example5*>(data);
	ASSERT_TRUE(ex->head != nullptr);

	ASSERT_TRUE(ex->head->left != nullptr);
	ASSERT_TRUE(ex->head->left->value != nullptr);
	ASSERT_STREQ("John", ex->head->left->value->name);
	ASSERT_EQ(44, ex->head->left->value->age);
	ASSERT_TRUE(ex->head->left->right == nullptr);
	ASSERT_TRUE(ex->head->left->left != nullptr);
	ASSERT_TRUE(ex->head->left->left->value != nullptr);
	ASSERT_STREQ("Victor", ex->head->left->left->value->name);
	ASSERT_EQ(400, ex->head->left->left->value->age);

	ASSERT_TRUE(ex->head->right != nullptr);
	ASSERT_TRUE(ex->head->right->value != nullptr);
	ASSERT_STREQ("Peter", ex->head->right->value->name);
	ASSERT_EQ(55, ex->head->right->value->age);
	ASSERT_TRUE(ex->head->right->left == nullptr);
	ASSERT_TRUE(ex->head->right->right == nullptr);
}

/*********** example 6 ************************/
static const char *example6_descriptor = "Tsample={DD v1 v2};[lsample;";

static const char *avpr_example6_descriptor = "{\
                \"protocol\" : \"types\", \
                \"namespace\" : \"test.ns\", \
                \"version\" : \"1.0.0\", \
                \"types\" : [ { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Double\", \
                    \"alias\" : \"double\", \
                    \"size\" : 8 \
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"sample\", \
                    \"fields\" : [{\
                        \"name\" : \"v1\", \
                        \"type\" : \"Double\" \
                    }, {\
                        \"name\" : \"v2\", \
                        \"type\" : \"Double\" \
                    }]\
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"structF\", \
                    \"fields\" : [{\
                        \"name\" : \"samples\", \
                        \"type\" : { \
                            \"type\" : \"array\",\
                            \"items\" : \"sample\"\
                        }\
                    }]\
                }], \
                \"messages\" : {} \
            }";
static const char *avpr_example6_fqn = "test.ns.structF";

static const char *example6_input = R"([{"v1":0.1,"v2":0.2},{"v1":1.1,"v2":1.2},{"v1":2.1,"v2":2.2}])";
static const char *avpr_example6_input = R"({
    "samples" : [{"v1":0.1,"v2":0.2},{"v1":1.1,"v2":1.2},{"v1":2.1,"v2":2.2}]
})";

struct ex6_sample {
	double v1;
	double v2;
};

struct ex6_sequence {
	uint32_t cap;
	uint32_t len;
	struct ex6_sample *buf;
};

struct ex6_avpr_struct {
    struct ex6_sequence samples;
};

static void check_example6(struct ex6_sequence seq) {
	ASSERT_EQ(3, seq.cap);
	ASSERT_EQ(3, seq.len);
	ASSERT_EQ(0.1, seq.buf[0].v1);
	ASSERT_EQ(0.2, seq.buf[0].v2);
	ASSERT_EQ(1.1, seq.buf[1].v1);
	ASSERT_EQ(1.2, seq.buf[1].v2);
	ASSERT_EQ(2.1, seq.buf[2].v1);
	ASSERT_EQ(2.2, seq.buf[2].v2);
}

static void check_example6_avpr(void *data) {
    auto ex = static_cast<ex6_avpr_struct*>(data);
	ASSERT_EQ(3, ex->samples.cap);
	ASSERT_EQ(3, ex->samples.len);
	ASSERT_EQ(0.1, ex->samples.buf[0].v1);
	ASSERT_EQ(0.2, ex->samples.buf[0].v2);
	ASSERT_EQ(1.1, ex->samples.buf[1].v1);
	ASSERT_EQ(1.2, ex->samples.buf[1].v2);
	ASSERT_EQ(2.1, ex->samples.buf[2].v1);
	ASSERT_EQ(2.2, ex->samples.buf[2].v2);
}

/*********** example 7 ************************/
const char *example7_descriptor = "{t a}";

static const char *avpr_example7_descriptor = "{\
                \"protocol\" : \"types\", \
                \"namespace\" : \"test.ns\", \
                \"version\" : \"1.0.0\", \
                \"types\" : [ { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Str\", \
                    \"size\" : 8, \
                    \"alias\" : \"string\" \
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"structG\", \
                    \"fields\" : [{\
                        \"name\" : \"a\", \
                        \"type\" : \"Str\" \
                    }]\
                }], \
                \"messages\" : {} \
            }";


static const char *avpr_example7_fqn = "test.ns.structG";

const char *example7_input = "{ \
    \"a\" : \"apache celix\" \
}";

struct example7 {
	char* a;   //0
};

static void check_example7(void *data) {
    auto ex = static_cast<example7*>(data);
	ASSERT_STREQ("apache celix", ex->a);
}


/*********** example 8 ************************/

const char *example8_descriptor = "{ZbijNP a b c d e f}";

static const char *avpr_example8_descriptor = "{\
                \"protocol\" : \"types\", \
                \"namespace\" : \"test.ns\", \
                \"version\" : \"1.0.0\", \
                \"types\" : [ { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Bool\", \
                    \"alias\" : \"boolean\", \
                    \"size\" : 1 \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"UChar\", \
                    \"size\" : 1 \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Uint\", \
                    \"size\" : 4 \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Ulong\", \
                    \"size\" : 8 \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Sint\", \
                    \"size\" : 4, \
                    \"signed\" : true \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"VoidPtr\", \
                    \"size\" : 1, \
                    \"alias\" : \"void_ptr\" \
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"structH\", \
                    \"fields\" : [{\
                        \"name\" : \"a\", \
                        \"type\" : \"Bool\" \
                    }, {\
                        \"name\" : \"b\", \
                        \"type\" : \"UChar\" \
                    }, {\
                        \"name\" : \"c\", \
                        \"type\" : \"Uint\" \
                    }, {\
                        \"name\" : \"d\", \
                        \"type\" : \"Ulong\" \
                    }, {\
                        \"name\" : \"e\", \
                        \"type\" : \"Sint\" \
                    }, {\
                        \"name\" : \"f\", \
                        \"type\" : \"VoidPtr\" \
                    }]\
                }], \
                \"messages\" : {} \
            }";


static const char *avpr_example8_fqn = "test.ns.structH";

const char *example8_input = "{ \
    \"a\" : true, \
    \"b\" : 4, \
    \"c\" : 8, \
    \"d\" : 16, \
    \"e\" : 32 \
}";

struct example8 {
	bool a;
	unsigned char b;
	uint32_t c;
	uint64_t d;
	int e;
	void* f;
};

static void check_example8(void *data) {
    auto ex = static_cast<example8*>(data);
	ASSERT_EQ(true,ex->a);
	ASSERT_EQ(4,ex->b);
	ASSERT_EQ(8,ex->c);
	//error on mac ASSERT_EQ(16,ex->d);
    ASSERT_TRUE(16 == ex->d);
	ASSERT_EQ(32,ex->e);
}

/*********** example 9 ************************/
const char *example9_descriptor = "{It#OK=0;#NOK=1;#MAYBE=2;E id name result}";

static const char *avpr_example9_descriptor = "{\
                \"protocol\" : \"types\", \
                \"namespace\" : \"test.ns\", \
                \"version\" : \"2.0.0\", \
                \"types\" : [ { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Sint\", \
                    \"size\" : 4, \
                    \"signed\" : true \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"String\", \
                    \"alias\" : \"string\", \
                    \"size\" : 8 \
                }, { \
                    \"type\" : \"enum\", \
                    \"name\" : \"ResultEnum\", \
                    \"EnumValues\" : [ \"OK = 0\", \"NOK=  1\", \"MAYBE  =2\" ],\
                    \"symbols\" : [\"OK\", \"NOK\", \"MAYBE\" ]\
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"structI\", \
                    \"fields\" : [{\
                        \"name\" : \"id\", \
                        \"type\" : \"Sint\" \
                    }, {\
                        \"name\" : \"name\", \
                        \"type\" : \"String\" \
                    }, {\
                        \"name\" : \"result\", \
                        \"type\" : \"ResultEnum\" \
                    }]\
                }], \
                \"messages\" : {} \
            }";

static const char *avpr_example9_fqn = "test.ns.structI";

const char *example9_input_1 = "{\
                                \"id\"   : 1000, \
                                \"name\" : \"my_name\", \
                                \"result\" : \"NOK\" \
                                }";

const char *example9_input_2 = "{\
                                \"id\"   : 1001, \
                                \"name\" : \"your_name\", \
                                \"result\" : \"MAYBE\" \
                                }";

const char *example9_input_3 =
        R"({
            "id" : 1001,
            "name" : "your_name",
            "result" : "OK"
        })";

enum ResultEnum {
    RE_OK = 0,
    RE_NOK = 1,
    RE_MAYBE = 2
};

struct example9 {
    int32_t id;
    char * name;
    enum ResultEnum result;
};

static void check_example9_1(void *data) {
    auto ex = static_cast<example9*>(data);
    ASSERT_EQ(1000, ex->id);
    ASSERT_STREQ("my_name", ex->name);
    ASSERT_EQ(RE_NOK, ex->result);
}

static void check_example9_2(void *data) {
    auto ex = static_cast<example9*>(data);
    ASSERT_EQ(1001, ex->id);
    ASSERT_STREQ("your_name", ex->name);
    ASSERT_EQ(RE_MAYBE, ex->result);
}

static void check_example9_3(void *data) {
    auto ex = static_cast<example9*>(data);
    ASSERT_EQ(1001, ex->id);
    ASSERT_STREQ("your_name", ex->name);
    ASSERT_EQ(RE_OK, ex->result);
}
/*********** example A ************************/
const char *exampleA_descriptor = "TPoint={DD x y};{lPoint;lPoint;t point_a point_b name}";

const char *exampleA_input =
		R"({
"point_a" : { "x" : 1.0, "y" : 2.0 },
"point_b" : { "x" : 3.0, "y" : 4.0 },
"name" : "this_is_my_name"
})";

struct exA_point {
	double x;
	double y;
};

struct exA_struct {
	exA_point point_a;
	exA_point point_b;
	char* name;
};

static void check_exampleA(void *data) {
	auto inp = static_cast<exA_struct*>(data);
	ASSERT_EQ(1.0, inp->point_a.x);
	ASSERT_EQ(2.0, inp->point_a.y);
	ASSERT_EQ(3.0, inp->point_b.x);
	ASSERT_EQ(4.0, inp->point_b.y);
	ASSERT_STREQ("this_is_my_name", inp->name);
}


static void parseAvprTests() {
	dyn_type *type;
	void *inst;
    int rc;

    inst = nullptr;
    type = dynType_parseAvprWithStr(avpr_example1_descriptor, avpr_example1_fqn);
    ASSERT_TRUE(type != nullptr);
	rc = jsonSerializer_deserialize(type, example1_input, strlen(example1_input), &inst);
	ASSERT_EQ(0, rc);
	check_example1(inst);
	dynType_free(type, inst);
	dynType_destroy(type);

    inst = nullptr;
    type = dynType_parseAvprWithStr(avpr_example2_descriptor, avpr_example2_fqn);
    ASSERT_TRUE(type != nullptr);
	rc = jsonSerializer_deserialize(type, example2_input, strlen(example2_input), &inst);
	ASSERT_EQ(0, rc);
	check_example2(inst);
	dynType_free(type, inst);
	dynType_destroy(type);

    inst = nullptr;
    type = dynType_parseAvprWithStr(avpr_example3_descriptor, avpr_example3_fqn);
    ASSERT_TRUE(type != nullptr);
	rc = jsonSerializer_deserialize(type, example3_input, strlen(example3_input), &inst);
	ASSERT_EQ(0, rc);
	check_example3(inst);
	dynType_free(type, inst);
	dynType_destroy(type);

    inst = nullptr;
    type = dynType_parseAvprWithStr(avpr_example4_descriptor, avpr_example4_fqn);
    ASSERT_TRUE(type != nullptr);
	rc = jsonSerializer_deserialize(type, example4_input, strlen(example4_input), &inst);
	ASSERT_EQ(0, rc);
	check_example4(inst);
	dynType_free(type, inst);
	dynType_destroy(type);

    inst = nullptr;
    type = dynType_parseAvprWithStr(avpr_example5_descriptor, avpr_example5_fqn);
    ASSERT_TRUE(type != nullptr);
	rc = jsonSerializer_deserialize(type, example5_input, strlen(example5_input), &inst);
	ASSERT_EQ(0, rc);
	check_example5(inst);
	dynType_free(type, inst);
	dynType_destroy(type);

    // Test 6 has custom checker because avdl does not allow an array to be a type on its own
    inst = nullptr;
    type = dynType_parseAvprWithStr(avpr_example6_descriptor, avpr_example6_fqn);
    ASSERT_TRUE(type != nullptr);
	rc = jsonSerializer_deserialize(type, avpr_example6_input, strlen(avpr_example6_input), &inst);
	ASSERT_EQ(0, rc);
	check_example6_avpr(inst);
	dynType_free(type, inst);
	dynType_destroy(type);

    inst = nullptr;
    type = dynType_parseAvprWithStr(avpr_example7_descriptor, avpr_example7_fqn);
    ASSERT_TRUE(type != nullptr);
	rc = jsonSerializer_deserialize(type, example7_input, strlen(example7_input), &inst);
	ASSERT_EQ(0, rc);
	check_example7(inst);
	dynType_free(type, inst);
	dynType_destroy(type);

    inst = nullptr;
    type = dynType_parseAvprWithStr(avpr_example8_descriptor, avpr_example8_fqn);
    ASSERT_TRUE(type != nullptr);
	rc = jsonSerializer_deserialize(type, example8_input, strlen(example8_input), &inst);
	ASSERT_EQ(0, rc);
	check_example8(inst);
	dynType_free(type, inst);
	dynType_destroy(type);

    inst = nullptr;
    type = dynType_parseAvprWithStr(avpr_example9_descriptor, avpr_example9_fqn);
    ASSERT_TRUE(type != nullptr);
	rc = jsonSerializer_deserialize(type, example9_input_1, strlen(example9_input_1), &inst);
	ASSERT_EQ(0, rc);
	check_example9_1(inst);
	dynType_free(type, inst);
	rc = jsonSerializer_deserialize(type, example9_input_2, strlen(example9_input_2), &inst);
	ASSERT_EQ(0, rc);
	check_example9_2(inst);
	dynType_free(type, inst);
    rc = jsonSerializer_deserialize(type, example9_input_3, strlen(example9_input_3), &inst);
    ASSERT_EQ(0, rc);
    check_example9_3(inst);
    dynType_free(type, inst);
	dynType_destroy(type);
}

static void parseTests() {
	dyn_type *type;
	void *inst;
	int rc;

	type = nullptr;
	inst = nullptr;
	rc = dynType_parseWithStr(example1_descriptor, nullptr, nullptr, &type);
	ASSERT_EQ(0, rc);
	rc = jsonSerializer_deserialize(type, example1_input, strlen(example1_input), &inst);
	ASSERT_EQ(0, rc);
	check_example1(inst);
	dynType_free(type, inst);
	dynType_destroy(type);

	type = nullptr;
	inst = nullptr;
	rc = dynType_parseWithStr(example2_descriptor, nullptr, nullptr, &type);
	ASSERT_EQ(0, rc);
	rc = jsonSerializer_deserialize(type, example2_input, strlen(example2_input), &inst);
	ASSERT_EQ(0, rc);
	check_example2(inst);
	dynType_free(type, inst);
	dynType_destroy(type);

	type = nullptr;
	inst = nullptr;
	rc = dynType_parseWithStr(example3_descriptor, nullptr, nullptr, &type);
	ASSERT_EQ(0, rc);
	rc = jsonSerializer_deserialize(type, example3_input, strlen(example3_input), &inst);
	ASSERT_EQ(0, rc);
	check_example3(inst);
	dynType_free(type, inst);
	dynType_destroy(type);

	type = nullptr;
	inst = nullptr;
	rc = dynType_parseWithStr(example4_descriptor, nullptr, nullptr, &type);
	ASSERT_EQ(0, rc);
	rc = jsonSerializer_deserialize(type, example4_input, strlen(example4_input), &inst);
	ASSERT_EQ(0, rc);
	check_example4(inst);
	dynType_free(type, inst);
	dynType_destroy(type);

	type = nullptr;
	inst = nullptr;
	rc = dynType_parseWithStr(example5_descriptor, nullptr, nullptr, &type);
	ASSERT_EQ(0, rc);
	rc = jsonSerializer_deserialize(type, example5_input, strlen(example5_input), &inst);
	ASSERT_EQ(0, rc);
	check_example5(inst);
	dynType_free(type, inst);
	dynType_destroy(type);

	type = nullptr;
	struct ex6_sequence *seq;
	rc = dynType_parseWithStr(example6_descriptor, nullptr, nullptr, &type);
	ASSERT_EQ(0, rc);
	rc = jsonSerializer_deserialize(type, example6_input, strlen(example6_input), (void **)&seq);
	ASSERT_EQ(0, rc);
	check_example6((*seq));
	dynType_free(type, seq);
	dynType_destroy(type);

	type = nullptr;
	inst = nullptr;
	rc = dynType_parseWithStr(example7_descriptor, nullptr, nullptr, &type);
	ASSERT_EQ(0, rc);
	rc = jsonSerializer_deserialize(type, example7_input, strlen(example7_input), &inst);
	ASSERT_EQ(0, rc);
	check_example7(inst);
	dynType_free(type, inst);
	dynType_destroy(type);

	type = nullptr;
	inst = nullptr;
	rc = dynType_parseWithStr(example8_descriptor, nullptr, nullptr, &type);
	ASSERT_EQ(0, rc);
	rc = jsonSerializer_deserialize(type, example8_input, strlen(example8_input), &inst);
	ASSERT_EQ(0, rc);
	check_example8(inst);
	dynType_free(type, inst);
	dynType_destroy(type);

	type = nullptr;
	inst = nullptr;
	rc = dynType_parseWithStr(example9_descriptor, nullptr, nullptr, &type);
	ASSERT_EQ(0, rc);
	rc = jsonSerializer_deserialize(type, example9_input_1, strlen(example9_input_1), &inst);
	ASSERT_EQ(0, rc);
	check_example9_1(inst);
	dynType_free(type, inst);
	rc = jsonSerializer_deserialize(type, example9_input_2, strlen(example9_input_2), &inst);
	ASSERT_EQ(0, rc);
	check_example9_2(inst);
	dynType_free(type, inst);
	dynType_destroy(type);

    type = nullptr;
    inst = nullptr;
    rc = dynType_parseWithStr(exampleA_descriptor, nullptr, nullptr, &type);
    ASSERT_EQ(0, rc);
    rc = jsonSerializer_deserialize(type, exampleA_input, strlen(exampleA_input), &inst);
    ASSERT_EQ(0, rc);
    check_exampleA(inst);
    dynType_free(type, inst);
    dynType_destroy(type);

    //parse ref by value
    type = nullptr;
    inst = nullptr;
    rc = dynType_parseWithStr("Ttype={DD a b};ltype;", nullptr, nullptr, &type);
    ASSERT_EQ(0, rc);
    auto inputStr = R"({"a":1.0, "b":2.0})";
    json_error_t error;
    json_auto_t *input = json_loadb(inputStr, strlen(inputStr), JSON_DECODE_ANY, &error);
    rc = jsonSerializer_deserializeJson(type, input, &inst);
    ASSERT_EQ(0, rc);
    struct {
        double a;
        double b;
    } *data = static_cast<decltype(data)>(inst);
    ASSERT_EQ(1.0, data->a);
    ASSERT_EQ(2.0, data->b);
    dynType_free(type, inst);
    dynType_destroy(type);

    //invalid input
    type = nullptr;
    inst = nullptr;
    rc = dynType_parseWithStr("{DD a b}", nullptr, nullptr, &type);
    ASSERT_EQ(0, rc);
    rc = jsonSerializer_deserialize(type, "invalid", strlen("invalid"), &inst);
    ASSERT_EQ(1, rc);
    celix_err_printErrors(stderr, nullptr, nullptr);
    dynType_destroy(type);

    //pointer type mismatch
    rc = dynType_parseWithStr("{*t a}", nullptr, nullptr, &type);
    ASSERT_EQ(0, rc);
    inputStr = R"({"a":1.0})";
    rc = jsonSerializer_deserialize(type, inputStr, strlen(inputStr), &inst);
    ASSERT_EQ(1, rc);
    celix_err_printErrors(stderr, nullptr, nullptr);
    dynType_destroy(type);

    //text type mismatch
    rc = dynType_parseWithStr("{t a}", nullptr, nullptr, &type);
    ASSERT_EQ(0, rc);
    inputStr = R"({"a":1.0})";
    rc = jsonSerializer_deserialize(type, inputStr, strlen(inputStr), &inst);
    ASSERT_EQ(1, rc);
    celix_err_printErrors(stderr, nullptr, nullptr);
    dynType_destroy(type);

    //enum type mismatch
    rc = dynType_parseWithStr("{#v1=1;#v2=2;E a}", nullptr, nullptr, &type);
    ASSERT_EQ(0, rc);
    inputStr = R"({"a":1.0})";
    rc = jsonSerializer_deserialize(type, inputStr, strlen(inputStr), &inst);
    ASSERT_EQ(1, rc);
    celix_err_printErrors(stderr, nullptr, nullptr);
    dynType_destroy(type);

    //enum value unknown
    rc = dynType_parseWithStr("{#v1=1;#v2=2;E a}", nullptr, nullptr, &type);
    ASSERT_EQ(0, rc);
    inputStr = R"({"a":"v3"})";
    rc = jsonSerializer_deserialize(type, inputStr, strlen(inputStr), &inst);
    ASSERT_EQ(1, rc);
    celix_err_printErrors(stderr, nullptr, nullptr);
    dynType_destroy(type);

    //sequence element type mismatch
    rc = dynType_parseWithStr("[t", nullptr, nullptr, &type);
    ASSERT_EQ(0, rc);
    inputStr = R"([1.0, 2.0])";
    rc = jsonSerializer_deserialize(type, inputStr, strlen(inputStr), &inst);
    ASSERT_EQ(1, rc);
    celix_err_printErrors(stderr, nullptr, nullptr);
    dynType_destroy(type);

    //unsupported untyped pointer
    rc = dynType_parseWithStr("{P a}", nullptr, nullptr, &type);
    ASSERT_EQ(0, rc);
    inputStr = R"({"a":1.0})";
    rc = jsonSerializer_deserialize(type, inputStr, strlen(inputStr), &inst);
    ASSERT_EQ(1, rc);
    celix_err_printErrors(stderr, nullptr, nullptr);
    dynType_destroy(type);
}

/*********** write example 1 ************************/
const char *write_example1_descriptor = "{BSIJsijFDNZb a b c d e f g h i j k l}";

const char *avpr_write_example1_descriptor = "{\
                \"protocol\" : \"types\", \
                \"namespace\" : \"test.ns\", \
                \"version\" : \"1.0.0\", \
                \"types\" : [ { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"SChar\", \
                    \"signed\" : true, \
                    \"size\" : 1 \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Short\", \
                    \"size\" : 2, \
                    \"signed\" : true \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Sint\", \
                    \"size\" : 4, \
                    \"signed\" : true \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Slong\", \
                    \"size\" : 8, \
                    \"signed\" : true \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"UShort\", \
                    \"size\" : 2 \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Uint\", \
                    \"size\" : 4 \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Ulong\", \
                    \"size\" : 8 \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Float\", \
                    \"size\" : 4, \
                    \"alias\" : \"float\" \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Double\", \
                    \"size\" : 8, \
                    \"alias\" : \"double\" \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"NativeInt\", \
                    \"size\" : 4, \
                    \"alias\" : \"native_int\" \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Bool\", \
                    \"size\" : 1, \
                    \"alias\" : \"boolean\" \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"UChar\", \
                    \"size\" : 1 \
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"structAW\", \
                    \"fields\" : [{\
                        \"name\" : \"a\", \
                        \"type\" : \"SChar\" \
                    }, {\
                        \"name\" : \"b\", \
                        \"type\" : \"Short\" \
                    }, {\
                        \"name\" : \"c\", \
                        \"type\" : \"Sint\" \
                    }, {\
                        \"name\" : \"d\", \
                        \"type\" : \"Slong\" \
                    }, {\
                        \"name\" : \"e\", \
                        \"type\" : \"UShort\" \
                    }, {\
                        \"name\" : \"f\", \
                        \"type\" : \"Uint\" \
                    }, {\
                        \"name\" : \"g\", \
                        \"type\" : \"Ulong\" \
                    }, {\
                        \"name\" : \"h\", \
                        \"type\" : \"Float\" \
                    }, {\
                        \"name\" : \"i\", \
                        \"type\" : \"Double\" \
                    }, {\
                        \"name\" : \"j\", \
                        \"type\" : \"NativeInt\" \
                    }, {\
                        \"name\" : \"k\", \
                        \"type\" : \"Bool\" \
                    }, {\
                        \"name\" : \"l\", \
                        \"type\" : \"UChar\" \
                    }]\
                }], \
                \"messages\" : {} \
            }";

const char *avpr_write_example1_fqn = "test.ns.structAW";

struct write_example1 {
	char a;
	int16_t b;
	int32_t c;
	int64_t d;
	uint16_t e;
	uint32_t f;
	uint64_t g;
	float h;
	double i;
	int j;
	bool k;
	unsigned char l;
};

void writeTest1(void) {
	write_example1 ex1 {'A',2,3,4,5,6,7,8.8f,9.9,10,true,12};
	ex1.k = !ex1.k;

	dyn_type *type = nullptr;
	char *result = nullptr;
	int rc = dynType_parseWithStr(write_example1_descriptor, "ex1", nullptr, &type);
	ASSERT_EQ(0, rc);
	rc = jsonSerializer_serialize(type, &ex1, &result);
	ASSERT_EQ(0, rc);
	ASSERT_TRUE(strstr(result, R"("b":2)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("c":3)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("d":4)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("e":5)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("f":6)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("g":7)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("h":8.8)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("i":9.9)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("j":10)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("k":false)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("l":12)") != nullptr);
	//printf("example 1 result: '%s'\n", result);
	dynType_destroy(type);
	free(result);
}

void writeAvprTest1(void) {
	write_example1 ex1 {1,2,3,4,5,6,7,8.8f,9.9,10,true,12};

	char *result = nullptr;
	dyn_type *type = dynType_parseAvprWithStr(avpr_write_example1_descriptor, avpr_write_example1_fqn);
	ASSERT_TRUE(type != nullptr);

	int rc = jsonSerializer_serialize(type, &ex1, &result);
	ASSERT_EQ(0, rc);

	ASSERT_TRUE(strstr(result, R"("a":1)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("b":2)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("c":3)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("d":4)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("e":5)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("f":6)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("g":7)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("h":8.8)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("i":9.9)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("j":10)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("k":true)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("l":12)") != nullptr);

	dynType_destroy(type);
	free(result);
}

/*********** write example 2 ************************/
const char *write_example2_descriptor = "{*{JJ a b}{SS c d} sub1 sub2}";

const char *avpr_write_example2_descriptor = "{\
                \"protocol\" : \"types\", \
                \"namespace\" : \"test.ns\", \
                \"version\" : \"1.0.0\", \
                \"types\" : [ { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Short\", \
                    \"size\" : 2, \
                    \"signed\" : true \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Slong\", \
                    \"size\" : 8, \
                    \"signed\" : true \
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"SubPtr\", \
                    \"fields\" : [{\
                        \"name\" : \"a\", \
                        \"type\" : \"Slong\" \
                    }, {\
                        \"name\" : \"b\", \
                        \"type\" : \"Slong\" \
                    }]\
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"Sub\", \
                    \"fields\" : [{\
                        \"name\" : \"c\", \
                        \"type\" : \"Short\" \
                    }, {\
                        \"name\" : \"d\", \
                        \"type\" : \"Short\" \
                    }]\
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"structBW\", \
                    \"fields\" : [{\
                        \"name\" : \"sub1\", \
                        \"ptr\" : true, \
                        \"type\" : \"SubPtr\" \
                    }, {\
                        \"name\" : \"sub2\", \
                        \"type\" : \"Sub\" \
                    }]\
                }], \
                \"messages\" : {} \
            }";

const char *avpr_write_example2_fqn = "test.ns.structBW";

struct write_example2_sub {
	int64_t a;
	int64_t b;
};

struct write_example2 {
	write_example2_sub *sub1;
	struct {
		int16_t c;
		int16_t d;
	};
};

void writeTest2(void) {
	write_example2_sub sub1 {1,2};
	write_example2 ex {&sub1, {3, 4}};

	dyn_type *type = nullptr;
	char *result = nullptr;
	int rc = dynType_parseWithStr(write_example2_descriptor, "ex2", nullptr, &type);
	ASSERT_EQ(0, rc);
	rc = jsonSerializer_serialize(type, &ex, &result);
	ASSERT_EQ(0, rc);
	ASSERT_TRUE(strstr(result, "\"a\":1") != nullptr);
    ASSERT_TRUE(strstr(result, "\"b\":2") != nullptr);
    ASSERT_TRUE(strstr(result, "\"c\":3") != nullptr);
    ASSERT_TRUE(strstr(result, "\"d\":4") != nullptr);
	//printf("example 2 result: '%s'\n", result);
	dynType_destroy(type);
	free(result);
}

void writeAvprTest2(void) {
    write_example2_sub sub1 {0,0};
    write_example2 ex {&sub1, {3, 4}};
    ex.sub1->a = 1;
    ex.sub1->b = 2;

	char *result = nullptr;
	dyn_type *type = dynType_parseAvprWithStr(avpr_write_example2_descriptor, avpr_write_example2_fqn);
	ASSERT_TRUE(type != nullptr);

	int rc = jsonSerializer_serialize(type, &ex, &result);
	ASSERT_EQ(0, rc);

	ASSERT_TRUE(strstr(result, "\"a\":1") != nullptr);
    ASSERT_TRUE(strstr(result, "\"b\":2") != nullptr);
    ASSERT_TRUE(strstr(result, "\"c\":3") != nullptr);
    ASSERT_TRUE(strstr(result, "\"d\":4") != nullptr);

	dynType_destroy(type);
	free(result);
}

/*********** write example 3 ************************/
const char *write_example3_descriptor = "Tperson={ti name age};[Lperson;";

const char *avpr_write_example3_descriptor = "{\
                \"protocol\" : \"types\", \
                \"namespace\" : \"test.ns\", \
                \"version\" : \"1.0.0\", \
                \"types\" : [ { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Str\", \
                    \"size\" : 8, \
                    \"alias\" : \"string\" \
                }, { \
                    \"type\" : \"fixed\", \
                    \"name\" : \"Uint\", \
                    \"size\" : 4 \
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"PersonPtr\", \
                    \"fields\" : [{\
                        \"name\" : \"p\", \
                        \"type\" : \"Person\", \
                        \"ptr\" : true \
                    }]\
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"Person\", \
                    \"fields\" : [{\
                        \"name\" : \"name\", \
                        \"type\" : \"Str\" \
                    }, {\
                        \"name\" : \"age\", \
                        \"type\" : \"Uint\" \
                    }]\
                }, { \
                    \"type\" : \"record\", \
                    \"name\" : \"structCW\", \
                    \"fields\" : [{\
                        \"name\" : \"persons\", \
                        \"type\" : { \
                            \"type\" : \"array\",\
                            \"items\" : \"PersonPtr\"\
                        }\
                    }]\
                }], \
                \"messages\" : {} \
            }";

const char *avpr_write_example3_fqn = "test.ns.structCW";

struct write_example3_person {
	const char *name;
	uint32_t age;
};

struct write_example3 {
	uint32_t cap;
	uint32_t len;
	write_example3_person **buf;
};

void writeTest3(void) {
	write_example3_person p1 {"John", 33};
	write_example3_person p2 {"Peter", 44};
	write_example3_person p3 {"Carol", 55};
	write_example3_person p4 {"Elton", 66};

	write_example3 seq {4, 4, (struct write_example3_person **) calloc(4, sizeof(void *))};
	seq.buf[0] = &p1;
	seq.buf[1] = &p2;
	seq.buf[2] = &p3;
	seq.buf[3] = &p4;

	dyn_type *type = nullptr;
	char *result = nullptr;
	int rc = dynType_parseWithStr(write_example3_descriptor, "ex3", nullptr, &type);
	ASSERT_EQ(0, rc);
	rc = jsonSerializer_serialize(type, &seq, &result);
	ASSERT_EQ(0, rc);
	ASSERT_TRUE(strstr(result, "\"age\":33") != nullptr);
    ASSERT_TRUE(strstr(result, "\"age\":44") != nullptr);
    ASSERT_TRUE(strstr(result, "\"age\":55") != nullptr);
    ASSERT_TRUE(strstr(result, "\"age\":66") != nullptr);
	//printf("example 3 result: '%s'\n", result);
	free(seq.buf);
	dynType_destroy(type);
	free(result);
}

void writeAvprTest3(void) {
    write_example3_person p1 {"John", 33};
    write_example3_person p2 {"Peter", 44};
    write_example3_person p3 {"Carol", 55};
    write_example3_person p4 {"Elton", 66};

    write_example3 seq {4, 4, (write_example3_person **) calloc(4, sizeof(void *))};
    seq.buf[0] = &p1;
    seq.buf[1] = &p2;
    seq.buf[2] = &p3;
    seq.buf[3] = &p4;

	char *result = nullptr;
	dyn_type *type = dynType_parseAvprWithStr(avpr_write_example3_descriptor, avpr_write_example3_fqn);
    ASSERT_TRUE(type != nullptr);

	int rc = jsonSerializer_serialize(type, &seq, &result);
	ASSERT_EQ(0, rc);

    ASSERT_TRUE(strstr(result, "\"age\":33") != nullptr);
    ASSERT_TRUE(strstr(result, "\"age\":44") != nullptr);
    ASSERT_TRUE(strstr(result, "\"age\":55") != nullptr);
    ASSERT_TRUE(strstr(result, "\"age\":66") != nullptr);

	free(seq.buf);
	dynType_destroy(type);
	free(result);
}

void writeEnum(void) {
    dyn_type *type = nullptr;
    char *result = nullptr;
    int rc = dynType_parseWithStr(R"(#v1=1;#v2=2;E)", nullptr, nullptr, &type);
    ASSERT_EQ(0, rc);
    enum {
        v1 = 1,
        v2 = 2
    }enumVal = v2;
    rc = jsonSerializer_serialize(type, &enumVal, &result);
    ASSERT_EQ(0, rc);
    ASSERT_TRUE(strstr(result, R"(v2)") != nullptr);
    free(result);
    dynType_destroy(type);
}

void writeRefByVal(void) {
    dyn_type *type = nullptr;
    char *result = nullptr;
    int rc = dynType_parseWithStr(R"(Ttype={DD a b};ltype;)", nullptr, nullptr, &type);
    ASSERT_EQ(0, rc);
    struct {
        double a;
        double b;
    }input{1.0,2.0};
    rc = jsonSerializer_serialize(type, &input, &result);
    ASSERT_EQ(0, rc);
    ASSERT_TRUE(strstr(result, R"("a":1.0)") != nullptr);
    ASSERT_TRUE(strstr(result, R"("b":2.0)") != nullptr);
    free(result);
    dynType_destroy(type);
}

void writeSequenceFailed(void) {
    dyn_type *type = nullptr;
    char *result = nullptr;
    int rc = dynType_parseWithStr(R"([D)", nullptr, nullptr, &type);
    ASSERT_EQ(0, rc);
    double input[] = {1.0,2.0};
    struct {
        uint32_t cap;
        uint32_t len;
        double *buf;
    }seq{1,2,input};//cap < len
    rc = jsonSerializer_serialize(type, &seq, &result);
    ASSERT_EQ(1, rc);
    celix_err_printErrors(stderr, nullptr, nullptr);
    dynType_destroy(type);
}

void writeComplexFailed(void) {
    dyn_type *type = nullptr;
    char *result = nullptr;
    //has unnamed complex element
    int rc = dynType_parseWithStr(R"({II a})", nullptr, nullptr, &type);
    ASSERT_EQ(0, rc);
    struct {
        int32_t a;
        int32_t b;
    }input{1,2};
    rc = jsonSerializer_serialize(type, &input, &result);
    ASSERT_EQ(1, rc);
    celix_err_printErrors(stderr, nullptr, nullptr);
    dynType_destroy(type);
}

void writeEnumFailed(void) {
    dyn_type *type = nullptr;
    char *result = nullptr;
    int rc = dynType_parseWithStr(R"(#v1=1;#v2=2;E)", nullptr, nullptr, &type);
    ASSERT_EQ(0, rc);
    enum {
        v1 = 1,
        v2 = 2,
        v3 = 3
    }enumVal = v3;//schema only has v1 and v2
    rc = jsonSerializer_serialize(type, &enumVal, &result);
    ASSERT_EQ(1, rc);
    celix_err_printErrors(stderr, nullptr, nullptr);
    dynType_destroy(type);
}

} // extern "C"


class JsonSerializerTests : public ::testing::Test {
public:
    JsonSerializerTests() {
        int lvl = 1;
        dynCommon_logSetup(stdLog, nullptr, lvl);
        dynType_logSetup(stdLog, nullptr,lvl);
        dynAvprType_logSetup(stdLog, nullptr,lvl);
        dynTypeCommon_logSetup(stdLog, nullptr,lvl);
        jsonSerializer_logSetup(stdLog, nullptr, lvl);
    }
    ~JsonSerializerTests() override {
    }

};

TEST_F(JsonSerializerTests, ParseTests) {
	parseTests();
}

TEST_F(JsonSerializerTests, ParseAvprTests) {
    parseAvprTests();
}

TEST_F(JsonSerializerTests, WriteTest1) {
	writeTest1();
    writeAvprTest1();
}

TEST_F(JsonSerializerTests, WriteTest2) {
	writeTest2();
    writeAvprTest2();
}

TEST_F(JsonSerializerTests, WriteTest3) {
	writeTest3();
    writeAvprTest3();
}

TEST_F(JsonSerializerTests, WriteEnum) {
    writeEnum();
}

TEST_F(JsonSerializerTests, WriteRefByVal) {
    writeRefByVal();
}

TEST_F(JsonSerializerTests, WriteSequenceFailed) {
    writeSequenceFailed();
}

TEST_F(JsonSerializerTests, WriteComplexFailed) {
    writeComplexFailed();
}

TEST_F(JsonSerializerTests, WriteEnumFailed) {
    writeEnumFailed();
}
