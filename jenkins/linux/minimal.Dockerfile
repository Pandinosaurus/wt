FROM ubuntu:20.04

ARG USER_ID
ARG USER_NAME
ARG GROUP_ID
ARG GROUP_NAME
ARG THREAD_COUNT=1

ARG DEBIAN_FRONTEND=noninteractive

SHELL ["/bin/bash", "-c"]

WORKDIR /root

RUN groupadd -g ${GROUP_ID} ${GROUP_NAME}
RUN useradd -ms /bin/bash -u ${USER_ID} -g ${GROUP_NAME} ${USER_NAME}

RUN apt-get update && apt-get install --no-install-recommends -y \
    wget \
    ca-certificates \
    git \
    build-essential \
    cmake \
    ccache \
    libunwind-dev \
    tzdata \
 && rm -rf /var/lib/apt/lists/*

RUN BOOST_VERSION=1.71.0 ;\
    BOOSTDIR=boost_${BOOST_VERSION//[.]/_} ;\
    wget https://archives.boost.io/release/${BOOST_VERSION}/source/${BOOSTDIR}.tar.bz2 -O ${BOOSTDIR}.tar.bz2 \
 && tar xf ${BOOSTDIR}.tar.bz2 \
 && rm ${BOOSTDIR}.tar.bz2 \
 && (cd ${BOOSTDIR} \
     && ./bootstrap.sh \
     && ./b2 \
          --prefix=/opt/boost \
          --with-filesystem \
          --with-thread \
          --with-program_options \
          -j${THREAD_COUNT} \
          install) \
 && rm -rf ${BOOSTDIR}

USER ${USER_ID}

RUN cd "${HOME}" && wget -qO- https://get.pnpm.io/install.sh | bash -

RUN export PNPM_HOME="${HOME}/.local/share/pnpm"; \
    export PATH="${PNPM_HOME}:${PATH}"; \
    cd "${HOME}" \
 && pnpm env use --global lts \
 && pnpm config set store-dir "${HOME}/.pnpm-store"

USER root
