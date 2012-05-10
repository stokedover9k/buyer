# Agent corresponds to an ID written in the input file
# Brand is the brand number (starting at 1)

function prob_purchase
{
    INFILE=${1}
    AGENT=${2}
    BRAND=${3}

    AWK_PROGRAM="
      BEGIN { print \"### PURCHASE   Agent: ${AGENT}   Brand: ${BRAND}\" }
            { print \$${BRAND} }
      END   { }"

    echo "### program:" ${AWK_PROGRAM}

    egrep "^PURCH\s*\(a=${AGENT}" ${INFILE} | grep ":.*$" -o | grep "[^:]*" -o | \
	awk "${AWK_PROGRAM}"
}

############################ Main ##############################################

INFILE=${1}
BRAND=${2}
TMP_DIR=".plot_tmp_dir"
PLOT_SCRIPT="${TMP_DIR}/plot.pl"

USAGE="Usage: sh purch-plot.sh DATA_FILE BRAND_NUM"
if ! [ -f ${INFILE} ]; then
    echo "ERROR: no data file provided or can't read file." 
    echo ${USAGE}
    exit 1
fi
if [ "${BRAND}" == "" ]; then
    echo "ERROR: brand number (arg 2) missing." 
    echo ${USAGE}
    exit 1
fi

if ! [ -d ${TMP_DIR} ]; then
    mkdir ${TMP_DIR}
fi
echo -n "plot " > ${PLOT_SCRIPT}

for agent in {0..2}
do
    prob_purchase ${INFILE} ${agent} ${BRAND} > ${TMP_DIR}/purch_${agent}_${BRAND}

    if [ "${LAST_FILE}" != "" ]; then 
	echo ", \\" >> ${PLOT_SCRIPT}
    fi
    LAST_FILE="purch_${agent}_${BRAND}"
    echo -n "\"./${TMP_DIR}/${LAST_FILE}\" with lines title 'Agent ${agent}'" >> ${PLOT_SCRIPT}
done

gnuplot -persist ${PLOT_SCRIPT}
rm -r ${TMP_DIR}

