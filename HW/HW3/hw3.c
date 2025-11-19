#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

/* Error helper */
static void err_exit(const char *msg) {
    if (msg == NULL) _exit(EXIT_FAILURE);
    fprintf(stderr, "ERROR: %s\n", msg);
    exit(EXIT_FAILURE);
}

/* Instruction node */
typedef struct Node {
    char *label;
    char *mn;
    char *op1;
    char *op2;
    char *op3;
    struct Node *next;
} Node;

/* Simple list helpers */
static Node *new_node(void) {
    Node *p = (Node *)malloc(sizeof(Node));
    if (p == NULL) err_exit("malloc failed");
    p->label = NULL; p->mn = NULL; p->op1 = NULL; p->op2 = NULL; p->op3 = NULL; p->next = NULL;
    return p;
}
static void free_nodes(Node *head) {
    Node *p = head;
    while (p != NULL) {
        Node *t = p->next;
        if (p->label) free(p->label);
        if (p->mn) free(p->mn);
        if (p->op1) free(p->op1);
        if (p->op2) free(p->op2);
        if (p->op3) free(p->op3);
        free(p);
        p = t;
    }
}

/* Label table node */
typedef struct Lab {
    char *name;
    int idx;
    struct Lab *next;
} Lab;
static void add_label(Lab **h, const char *name, int idx) {
    Lab *n = (Lab *)malloc(sizeof(Lab));
    if (n == NULL) err_exit("malloc failed");
    n->name = strdup(name);
    if (n->name == NULL) err_exit("malloc failed");
    n->idx = idx;
    n->next = *h;
    *h = n;
}
static int find_label(Lab *h, const char *name) {
    Lab *p = h;
    while (p != NULL) {
        if (strcmp(p->name, name) == 0) return p->idx;
        p = p->next;
    }
    return -1;
}
static void free_labels(Lab *h) {
    Lab *p = h;
    while (p != NULL) {
        Lab *t = p->next;
        free(p->name);
        free(p);
        p = t;
    }
}

/* trim using pointer arithmetic */
static void trim(char *s) {
    if (s == NULL) return;
    int len = (int)strlen(s);
    while (len > 0 && isspace(*(s + len - 1))) { *(s + len - 1) = '\0'; len--; }
    int i = 0;
    while (*(s + i) && isspace(*(s + i))) i++;
    if (i > 0) memmove(s, s + i, (size_t)(len - i + 1));
}

/* remove comment starting at '#' */
static void remove_comment(char *s) {
    if (s == NULL) return;
    char *c = strchr(s, '#');
    if (c != NULL) *c = '\0';
}

/* duplicate token trimmed */
static char *duptrim(const char *x) {
    if (x == NULL) return NULL;
    char *t = strdup(x);
    if (t == NULL) err_exit("malloc failed");
    trim(t);
    if (strlen(t) == 0) { free(t); return NULL; }
    return t;
}

/* parse integer decimal (signed) */
static int parse_int(const char *s, int *out) {
    if (s == NULL) return -1;
    char *endp = NULL;
    long v = strtol(s, &endp, 10);
    if (endp == s) return -1;
    *out = (int)v;
    return 0;
}

/* register decoding */
static int regnum(const char *r) {
    if (r == NULL) return -1;
    if (*r != '$') return -1;
    if (strcmp(r, "$zero") == 0) return 0;
    if (strcmp(r, "$at") == 0) return 1;
    if (strcmp(r, "$v0") == 0) return 2;
    if (strcmp(r, "$v1") == 0) return 3;
    if (strcmp(r, "$a0") == 0) return 4;
    if (strcmp(r, "$a1") == 0) return 5;
    if (strcmp(r, "$a2") == 0) return 6;
    if (strcmp(r, "$a3") == 0) return 7;
    if (strcmp(r, "$t0") == 0) return 8;
    if (strcmp(r, "$t1") == 0) return 9;
    if (strcmp(r, "$t2") == 0) return 10;
    if (strcmp(r, "$t3") == 0) return 11;
    if (strcmp(r, "$t4") == 0) return 12;
    if (strcmp(r, "$t5") == 0) return 13;
    if (strcmp(r, "$t6") == 0) return 14;
    if (strcmp(r, "$t7") == 0) return 15;
    if (strcmp(r, "$s0") == 0) return 16;
    if (strcmp(r, "$s1") == 0) return 17;
    if (strcmp(r, "$s2") == 0) return 18;
    if (strcmp(r, "$s3") == 0) return 19;
    if (strcmp(r, "$s4") == 0) return 20;
    if (strcmp(r, "$s5") == 0) return 21;
    if (strcmp(r, "$s6") == 0) return 22;
    if (strcmp(r, "$s7") == 0) return 23;
    if (strcmp(r, "$t8") == 0) return 24;
    if (strcmp(r, "$t9") == 0) return 25;
    if (strcmp(r, "$k0") == 0) return 26;
    if (strcmp(r, "$k1") == 0) return 27;
    if (strcmp(r, "$gp") == 0) return 28;
    if (strcmp(r, "$sp") == 0) return 29;
    if (strcmp(r, "$fp") == 0) return 30;
    if (strcmp(r, "$ra") == 0) return 31;
    /* numeric registers like $0 .. $31 */
    if (*(r + 1) && isdigit(*(r + 1))) {
        int v = 0;
        const char *p = r + 1;
        while (*p) {
            if (!isdigit(*p)) return -1;
            v = v * 10 + (*p - '0');
            p++;
        }
        if (v >= 0 && v <= 31) return v;
    }
    return -1;
}

/* parse mem operand like -100($gp) */
static int parse_mem(const char *s, int *off, int *base) {
    if (s == NULL) return -1;
    char *op = strdup(s);
    if (op == NULL) err_exit("malloc failed");
    trim(op);
    char *lp = strchr(op, '(');
    char *rp = strchr(op, ')');
    if (lp == NULL || rp == NULL || rp < lp) { free(op); return -1; }
    int lenoff = (int)(lp - op);
    char *offstr = (char *)malloc((size_t)lenoff + 1);
    if (offstr == NULL) err_exit("malloc failed");
    memcpy(offstr, op, (size_t)lenoff);
    *(offstr + lenoff) = '\0';
    trim(offstr);
    int ov = 0;
    if (strlen(offstr) > 0) {
        if (parse_int(offstr, &ov) != 0) { free(offstr); free(op); return -1; }
    }
    free(offstr);
    int reglen = (int)(rp - (lp + 1));
    char *regstr = (char *)malloc((size_t)reglen + 1);
    if (regstr == NULL) err_exit("malloc failed");
    memcpy(regstr, lp + 1, (size_t)reglen);
    *(regstr + reglen) = '\0';
    trim(regstr);
    int r = regnum(regstr);
    free(regstr);
    free(op);
    if (r < 0) return -1;
    *off = ov; *base = r;
    return 0;
}

/* print 32-bit binary string */
static void print32(unsigned int w) {
    char *s = (char *)malloc(33);
    if (s == NULL) err_exit("malloc failed");
    char *p = s;
    int k = 31;
    while (k >= 0) {
        *p = ((w >> k) & 1u) ? '1' : '0';
        p++;
        k--;
    }
    *p = '\0';
    printf("%s\n", s);
    free(s);
}

/* Parse whole file text into a node list. */
static Node *parse_lines(const char *buf) {
    if (buf == NULL) return NULL;
    Node *head = NULL;
    Node *tail = NULL;
    const char *p = buf;
    while (*p != '\0') {
        const char *q = p;
        while (*q != '\0' && *q != '\n') q++;
        int linelen = (int)(q - p);
        char *line = (char *)malloc((size_t)linelen + 1);
        if (line == NULL) err_exit("malloc failed");
        memcpy(line, p, (size_t)linelen);
        *(line + linelen) = '\0';
        remove_comment(line);
        trim(line);
        if (strlen(line) > 0) {
            /* check label */
            char *colon = strchr(line, ':');
            char *labeltxt = NULL;
            char *rest = line;
            if (colon != NULL) {
                int L = (int)(colon - line);
                labeltxt = (char *)malloc((size_t)L + 1);
                if (labeltxt == NULL) err_exit("malloc failed");
                memcpy(labeltxt, line, (size_t)L);
                *(labeltxt + L) = '\0';
                trim(labeltxt);
                rest = colon + 1;
            }
            trim(rest);
            if (strlen(rest) > 0) {
                /* extract mnemonic */
                const char *s1 = rest;
                const char *s2 = s1;
                while (*s2 != '\0' && !isspace(*s2)) s2++;
                int mlen = (int)(s2 - s1);
                char *mn = (char *)malloc((size_t)mlen + 1);
                if (mn == NULL) err_exit("malloc failed");
                memcpy(mn, s1, (size_t)mlen);
                *(mn + mlen) = '\0';
                trim(mn);
                /* operands remainder */
                char *ops = NULL;
                if (*s2 != '\0') {
                    const char *r = s2;
                    while (*r != '\0' && isspace(*r)) r++;
                    int olen = (int)strlen(r);
                    if (olen > 0) {
                        ops = (char *)malloc((size_t)olen + 1);
                        if (ops == NULL) err_exit("malloc failed");
                        memcpy(ops, r, (size_t)olen);
                        *(ops + olen) = '\0';
                        trim(ops);
                    }
                }
                char *op1 = NULL; char *op2 = NULL; char *op3 = NULL;
                if (ops != NULL) {
                    /* split by commas manually */
                    char *cp = ops;
                    char *comma = strchr(cp, ',');
                    if (comma == NULL) {
                        op1 = duptrim(cp);
                    } else {
                        int tlen = (int)(comma - cp);
                        char *t = (char *)malloc((size_t)tlen + 1);
                        if (t == NULL) err_exit("malloc failed");
                        memcpy(t, cp, (size_t)tlen);
                        *(t + tlen) = '\0';
                        op1 = duptrim(t);
                        free(t);
                        cp = comma + 1;
                        comma = strchr(cp, ',');
                        if (comma == NULL) {
                            op2 = duptrim(cp);
                        } else {
                            tlen = (int)(comma - cp);
                            t = (char *)malloc((size_t)tlen + 1);
                            if (t == NULL) err_exit("malloc failed");
                            memcpy(t, cp, (size_t)tlen);
                            *(t + tlen) = '\0';
                            op2 = duptrim(t);
                            free(t);
                            cp = comma + 1;
                            op3 = duptrim(cp);
                        }
                    }
                }
                Node *n = new_node();
                if (labeltxt != NULL) n->label = labeltxt;
                n->mn = mn;
                if (op1) n->op1 = op1;
                if (op2) n->op2 = op2;
                if (op3) n->op3 = op3;
                if (head == NULL) { head = n; tail = n; } else { tail->next = n; tail = n; }
                if (ops) free(ops);
            } else {
                if (labeltxt) free(labeltxt);
            }
        }
        free(line);
        if (*q == '\0') break;
        p = q + 1;
    }
    return head;
}

/* Expand pseudoinstructions. */
static Node *expand_pseudolist(Node *orig) {
    Node *newh = NULL;
    Node *tail = NULL;
    Node *p = orig;
    char *buf = NULL; /* buffer for snprintf, allocated dynamically */
    char *buf2 = NULL; /* second buffer for li expansion */
    const size_t BUF_SIZE = 64;

    while (p != NULL) {
        const char *mn = p->mn;
        if (mn == NULL) { p = p->next; continue; }

        /* subi -> addi with negated immediate */
        if (strcmp(mn, "subi") == 0) {
            if (p->op1 == NULL || p->op2 == NULL || p->op3 == NULL) err_exit("subi operand error");
            int v = 0; if (parse_int(p->op3, &v) != 0) err_exit("invalid subi immediate");
            Node *r = new_node();
            if (p->label) r->label = strdup(p->label);
            r->mn = strdup("addi");
            r->op1 = strdup(p->op1);
            r->op2 = strdup(p->op2);
            /* Use dynamic buffer instead of array */
            buf = (char *)malloc(BUF_SIZE);
            if (buf == NULL) err_exit("malloc failed");
            snprintf(buf, BUF_SIZE, "%d", -v);
            r->op3 = strdup(buf);
            free(buf);
            if (newh == NULL) { newh = r; tail = r; } else { tail->next = r; tail = r; }
            p = p->next; continue;
        }
        /* move -> add $t,$s,$zero */
        if (strcmp(mn, "move") == 0) {
            if (p->op1 == NULL || p->op2 == NULL) err_exit("move operand error");
            Node *r = new_node();
            if (p->label) r->label = strdup(p->label);
            r->mn = strdup("add");
            r->op1 = strdup(p->op1);
            r->op2 = strdup(p->op2);
            r->op3 = strdup("$zero");
            if (newh == NULL) { newh = r; tail = r; } else { tail->next = r; tail = r; }
            p = p->next; continue;
        }
        /* li -> addi or lui+ori */
        if (strcmp(mn, "li") == 0) {
            if (p->op1 == NULL || p->op2 == NULL) err_exit("li operand error");
            int val = 0; if (parse_int(p->op2, &val) != 0) err_exit("invalid li immediate");
            if (val >= -32768 && val <= 32767) {
                Node *r = new_node();
                if (p->label) r->label = strdup(p->label);
                r->mn = strdup("addi");
                r->op1 = strdup(p->op1);
                r->op2 = strdup("$zero");
                /* Use dynamic buffer instead of array */
                buf = (char *)malloc(BUF_SIZE);
                if (buf == NULL) err_exit("malloc failed");
                snprintf(buf, BUF_SIZE, "%d", val);
                r->op3 = strdup(buf);
                free(buf);
                if (newh == NULL) { newh = r; tail = r; } else { tail->next = r; tail = r; }
            } else {
                unsigned int u = (unsigned int)val;
                unsigned int up = (u >> 16) & 0xFFFFu;
                unsigned int low = u & 0xFFFFu;
                Node *r1 = new_node();
                if (p->label) r1->label = strdup(p->label);
                r1->mn = strdup("lui");
                r1->op1 = strdup("$at");
                /* Use dynamic buffer instead of array */
                buf = (char *)malloc(BUF_SIZE);
                if (buf == NULL) err_exit("malloc failed");
                snprintf(buf, BUF_SIZE, "%u", up);
                r1->op2 = strdup(buf);
                free(buf); /* free buf for next use */

                Node *r2 = new_node();
                r2->mn = strdup("ori");
                r2->op1 = strdup(p->op1);
                r2->op2 = strdup("$at");
                /* Use dynamic buffer instead of array */
                buf2 = (char *)malloc(BUF_SIZE);
                if (buf2 == NULL) err_exit("malloc failed");
                snprintf(buf2, BUF_SIZE, "%u", low);
                r2->op3 = strdup(buf2);
                free(buf2);

                if (newh == NULL) { newh = r1; tail = r1; } else { tail->next = r1; tail = r1; }
                tail->next = r2; tail = r2;
            }
            p = p->next; continue;
        }
        /* conditional branches into slt + beq/bne */
        if (strcmp(mn, "bge") == 0 || strcmp(mn, "bgt") == 0 || strcmp(mn, "ble") == 0 || strcmp(mn, "blt") == 0) {
            if (p->op1 == NULL || p->op2 == NULL || p->op3 == NULL) err_exit("branch pseudo operand error");
            Node *slt = new_node();
            if (p->label) slt->label = strdup(p->label);
            slt->mn = strdup("slt");
            slt->op1 = strdup("$at");
            if (strcmp(mn, "bge") == 0 || strcmp(mn, "blt") == 0) {
                slt->op2 = strdup(p->op1); slt->op3 = strdup(p->op2);
            } else {
                slt->op2 = strdup(p->op2); slt->op3 = strdup(p->op1);
            }
            Node *br = new_node();
            if (strcmp(mn, "bge") == 0) br->mn = strdup("beq");
            else if (strcmp(mn, "bgt") == 0) br->mn = strdup("bne");
            else if (strcmp(mn, "ble") == 0) br->mn = strdup("beq");
            else br->mn = strdup("bne");
            br->op1 = strdup("$at"); br->op2 = strdup("$zero"); br->op3 = strdup(p->op3);
            if (newh == NULL) { newh = slt; tail = slt; } else { tail->next = slt; tail = slt; }
            tail->next = br; tail = br;
            p = p->next; continue;
        }
        /* default: copy */
        Node *c = new_node();
        if (p->label) c->label = strdup(p->label);
        c->mn = strdup(p->mn);
        if (p->op1) c->op1 = strdup(p->op1);
        if (p->op2) c->op2 = strdup(p->op2);
        if (p->op3) c->op3 = strdup(p->op3);
        if (newh == NULL) { newh = c; tail = c; } else { tail->next = c; tail = c; }
        p = p->next;
    }
    return newh;
}

/* Build label table from expanded instruction list: map label->index */
static Lab *build_labels(Node *h) {
    Lab *L = NULL;
    Node *p = h;
    int idx = 0;
    while (p != NULL) {
        if (p->label) add_label(&L, p->label, idx);
        if (p->mn) idx++;
        p = p->next;
    }
    return L;
}

/* encode & print one node (with label table for branches/jumps) */
static void encode_print(Node *n, int idx, Lab *labels) {
    if (n == NULL || n->mn == NULL) return;
    const char *mn = n->mn;
    unsigned int word = 0u;
    /* R-type default */
    if (strcmp(mn, "add") == 0 || strcmp(mn, "addu") == 0 || strcmp(mn, "sub") == 0 ||
        strcmp(mn, "and") == 0 || strcmp(mn, "or") == 0 || strcmp(mn, "nor") == 0 ||
        strcmp(mn, "slt") == 0) {
        if (n->op1 == NULL || n->op2 == NULL || n->op3 == NULL) err_exit("R-type missing operand");
        int rd = regnum(n->op1); int rs = regnum(n->op2); int rt = regnum(n->op3);
        if (rd < 0 || rs < 0 || rt < 0) err_exit("invalid register");
        unsigned int funct = 0u;
        if (strcmp(mn, "add") == 0) funct = 0x20u;
        else if (strcmp(mn, "addu") == 0) funct = 0x21u;
        else if (strcmp(mn, "sub") == 0) funct = 0x22u;
        else if (strcmp(mn, "and") == 0) funct = 0x24u;
        else if (strcmp(mn, "or") == 0) funct = 0x25u;
        else if (strcmp(mn, "nor") == 0) funct = 0x27u;
        else funct = 0x2Au;
        word = (rs << 21) | (rt << 16) | (rd << 11) | funct;
    }
    else if (strcmp(mn, "sll") == 0 || strcmp(mn, "srl") == 0 || strcmp(mn, "sra") == 0) {
        if (n->op1 == NULL || n->op2 == NULL || n->op3 == NULL) err_exit("shift missing");
        int rd = regnum(n->op1); int rt = regnum(n->op2);
        int sh = 0; if (parse_int(n->op3, &sh) != 0) err_exit("invalid shamt");
        if (rd < 0 || rt < 0) err_exit("invalid register");
        unsigned int funct = (strcmp(mn, "sll") == 0) ? 0x00u : (strcmp(mn, "srl") == 0 ? 0x02u : 0x03u);
        word = (rt << 16) | (rd << 11) | ((sh & 0x1Fu) << 6) | funct;
    }
    else if (strcmp(mn, "jr") == 0) {
        if (n->op1 == NULL) err_exit("jr missing");
        int rs = regnum(n->op1);
        if (rs < 0) err_exit("invalid register");
        word = (rs << 21) | 0x08u;
    }
    else if (strcmp(mn, "mult") == 0 || strcmp(mn, "div") == 0) {
        if (n->op1 == NULL || n->op2 == NULL) err_exit("mult/div missing");
        int rs = regnum(n->op1); int rt = regnum(n->op2);
        if (rs < 0 || rt < 0) err_exit("invalid register");
        unsigned int funct = (strcmp(mn, "mult") == 0) ? 0x18u : 0x1Au;
        word = (rs << 21) | (rt << 16) | funct;
    }
    else if (strcmp(mn, "mfhi") == 0 || strcmp(mn, "mflo") == 0) {
        if (n->op1 == NULL) err_exit("mfhi/mflo missing");
        int rd = regnum(n->op1);
        if (rd < 0) err_exit("invalid register");
        unsigned int funct = (strcmp(mn, "mfhi") == 0) ? 0x10u : 0x12u;
        word = (rd << 11) | funct;
    }
    /* I-type and J-type */
    else if (strcmp(mn, "addi") == 0 || strcmp(mn, "andi") == 0 || strcmp(mn, "ori") == 0) {
        if (n->op1 == NULL || n->op2 == NULL || n->op3 == NULL) err_exit("I-type missing");
        int rt = regnum(n->op1); int rs = regnum(n->op2);
        if (rt < 0 || rs < 0) err_exit("invalid register");
        int imm = 0; if (parse_int(n->op3, &imm) != 0) err_exit("invalid immediate");
        unsigned int op = (strcmp(mn, "addi") == 0) ? 0x08u : (strcmp(mn, "andi") == 0 ? 0x0Cu : 0x0Du);
        word = (op << 26) | (rs << 21) | (rt << 16) | ((unsigned int)imm & 0xFFFFu);
    }
    else if (strcmp(mn, "lui") == 0) {
        if (n->op1 == NULL || n->op2 == NULL) err_exit("lui missing");
        int rt = regnum(n->op1); if (rt < 0) err_exit("invalid register");
        int imm = 0; if (parse_int(n->op2, &imm) != 0) err_exit("invalid immediate");
        word = (0x0Fu << 26) | (rt << 16) | ((unsigned int)imm & 0xFFFFu);
    }
    else if (strcmp(mn, "lw") == 0 || strcmp(mn, "sw") == 0) {
        if (n->op1 == NULL || n->op2 == NULL) err_exit("lw/sw missing");
        int rt = regnum(n->op1); if (rt < 0) err_exit("invalid register");
        int off = 0; int base = 0;
        if (parse_mem(n->op2, &off, &base) != 0) err_exit("invalid memory operand");
        unsigned int op = (strcmp(mn, "lw") == 0) ? 0x23u : 0x2Bu;
        word = (op << 26) | (base << 21) | (rt << 16) | ((unsigned int)off & 0xFFFFu);
    }
    else if (strcmp(mn, "beq") == 0 || strcmp(mn, "bne") == 0) {
        if (n->op1 == NULL || n->op2 == NULL || n->op3 == NULL) err_exit("branch missing");
        int rs = regnum(n->op1); int rt = regnum(n->op2);
        if (rs < 0 || rt < 0) err_exit("invalid register");
        int lab = find_label(labels, n->op3);
        if (lab < 0) err_exit("label not found");
        int offset = lab - (idx + 1);
        unsigned int op = (strcmp(mn, "beq") == 0) ? 0x04u : 0x05u;
        word = (op << 26) | (rs << 21) | (rt << 16) | ((unsigned int)offset & 0xFFFFu);
    }
    else if (strcmp(mn, "j") == 0 || strcmp(mn, "jal") == 0) {
        if (n->op1 == NULL) err_exit("jump missing");
        int lab = find_label(labels, n->op1);
        if (lab < 0) err_exit("label not found");
        unsigned int addr = 0x00400000u + (unsigned int)lab * 4u;
        unsigned int target = (addr >> 2) & 0x03FFFFFFu;
        unsigned int op = (strcmp(mn, "j") == 0) ? 0x02u : 0x03u;
        word = (op << 26) | target;
    }
    else {
        char *msg = (char *)malloc(128);
        if (msg == NULL) err_exit("malloc failed");
        snprintf(msg, 128, "unsupported mnemonic: %s", mn);
        err_exit(msg);
        free(msg); /* cleanup if err_exit is somehow circumvented */
    }
    print32(word);
}

/* main */
int main(int argc, char **argv) {
    if (argc != 2) err_exit("INVALID INPUT, USAGE: ./hw3.out <asm-file>");
    const char *fname = *(argv + 1);
    int fd = open(fname, O_RDONLY);
    if (fd < 0) { perror("ERROR: open failed"); return EXIT_FAILURE; }
    struct stat st;
    if (fstat(fd, &st) != 0) { close(fd); err_exit("fstat failed"); }
    off_t fsz = st.st_size;
    if (fsz < 0) { close(fd); err_exit("invalid file size"); }
    char *buf = (char *)malloc((size_t)fsz + 1);
    if (buf == NULL) { close(fd); err_exit("malloc failed"); }
    ssize_t rtot = 0;
    while ((off_t)rtot < fsz) {
        ssize_t r = read(fd, buf + rtot, (size_t)(fsz - rtot));
        if (r < 0) { free(buf); close(fd); err_exit("read failed"); }
        if (r == 0) break;
        rtot += r;
    }
    *(buf + rtot) = '\0';
    close(fd);

    /* parse original list (with node->label set when label exists on line) */
    Node *orig = parse_lines(buf);

    /* expand pseudoinstructions */
    Node *expanded = expand_pseudolist(orig);

    /* build final label table */
    Lab *labels = build_labels(expanded);

    /* encode and print */
    int idx = 0;
    Node *p = expanded;
    while (p != NULL) {
        if (p->mn != NULL) {
            encode_print(p, idx, labels);
            idx++;
        }
        p = p->next;
    }

    printf("end\n");

    /* cleanup */
    free(buf);
    free_nodes(orig);
    free_nodes(expanded);
    free_labels(labels);
    return EXIT_SUCCESS;
}