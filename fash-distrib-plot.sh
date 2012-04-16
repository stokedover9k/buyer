
function prob_fash_state
{
    INFILE=${1}
    AGENT=${2}
    BRAND=${3}
    FASH=${4}

    AWK_PROG="
      BEGIN { print \"### FASH STATE DISTRIBUTION\"
              print \"### Agent: ${AGENT}   Brand: ${BRAND}  State: ${FASH}\" }
            { print \$${BRAND} }
      END   { }"

    egrep "^F_DIST\s*\(a=${AGENT},f=${FASH}" ${INFILE} | \
	grep ":.*$" -o | grep "[^:]*" -o | \
	awk "${AWK_PROG}"
}

############################ Main ##############################################

INFILE=${1}
AGENT=${2}
BRAND=${3}
TMP_DIR=".plot_tmp_dir"
PLOT_SCRIPT="${TMP_DIR}/plot_fash_distrib.pl"

USAGE="Usage: sh fash-distrib-plot.sh DATA_FILE AGENT_ID BRAND_NUM"
if ! [ -f ${INFILE} ]; then
    echo "ERROR: no data file provided or can't read file." 
    echo ${USAGE}
    exit 1
fi
if [ "${AGENT}" == "" ]; then
    echo "ERROR: agent number (arg 2) missing." 
    echo ${USAGE}
    exit 1
fi
if [ "${BRAND}" == "" ]; then
    echo "ERROR: brand number (arg 3) missing." 
    echo ${USAGE}
    exit 1
fi

if ! [ -d ${TMP_DIR} ]; then
    mkdir ${TMP_DIR}
fi
echo -n "plot " > ${PLOT_SCRIPT}

for fash_state in {0..4}
do
    file_name=${TMP_DIR}/fash_distr_${AGENT}_${BRAND}_${fash_state}
    prob_fash_state ${INFILE} ${AGENT} ${BRAND} ${fash_state} \
	> ${file_name}

    if [ "${LAST_FILE}" != "" ]; then
	echo ", \\" >> ${PLOT_SCRIPT}
    fi
    LAST_FILE=${file_name}
    echo -n "\"./${file_name}\" with lines title 'Fash State: ${fash_state}'" >> ${PLOT_SCRIPT}
done

gnuplot -persist ${PLOT_SCRIPT}
rm -r ${TMP_DIR}