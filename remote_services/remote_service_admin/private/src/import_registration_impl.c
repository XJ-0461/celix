/**
 *Licensed to the Apache Software Foundation (ASF) under one
 *or more contributor license agreements.  See the NOTICE file
 *distributed with this work for additional information
 *regarding copyright ownership.  The ASF licenses this file
 *to you under the Apache License, Version 2.0 (the
 *"License"); you may not use this file except in compliance
 *with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *Unless required by applicable law or agreed to in writing,
 *software distributed under the License is distributed on an
 *"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 *specific language governing permissions and limitations
 *under the License.
 */
/*
 * import_registration_impl.c
 *
 *  \date       Oct 14, 2011
 *  \author    	<a href="mailto:celix-dev@incubator.apache.org">Apache Celix Project Team</a>
 *  \copyright	Apache License, Version 2.0
 */

#include <stdlib.h>

#include <apr_strings.h>

#include <constants.h>

#include "celix_errno.h"

#include "import_registration_impl.h"
#include "remote_service_admin_impl.h"
#include "remote_proxy.h"
#include "service_tracker.h"
#include "bundle_context.h"
#include "bundle.h"

celix_status_t importRegistration_proxyAdding(void * handle, SERVICE_REFERENCE reference, void **service);
celix_status_t importRegistration_proxyAdded(void * handle, SERVICE_REFERENCE reference, void *service);
celix_status_t importRegistration_proxyModified(void * handle, SERVICE_REFERENCE reference, void *service);
celix_status_t importRegistration_proxyRemoved(void * handle, SERVICE_REFERENCE reference, void *service);

celix_status_t importRegistration_createProxyTracker(import_registration_t registration, service_tracker_t *tracker);

celix_status_t importRegistration_create(apr_pool_t *pool, endpoint_description_t endpoint, remote_service_admin_t rsa, bundle_context_t context, import_registration_t *registration) {
	celix_status_t status = CELIX_SUCCESS;
	apr_pool_t *mypool = NULL;
	apr_pool_create(&mypool, pool);

	*registration = apr_palloc(mypool, sizeof(**registration));
	if (!*registration) {
		status = CELIX_ENOMEM;
	} else {
		(*registration)->pool = mypool;
		(*registration)->context = context;
		(*registration)->closed = false;
		(*registration)->endpointDescription = endpoint;
		(*registration)->rsa = rsa;
		(*registration)->proxy = NULL;
		(*registration)->reference = NULL;
		(*registration)->proxyTracker = NULL;
		(*registration)->bundle = NULL;
		(*registration)->importReference = NULL;
	}

	return status;
}

celix_status_t importRegistration_startTracking(import_registration_t registration) {
	celix_status_t status = CELIX_SUCCESS;

	if (registration->proxyTracker == NULL) {
		importRegistration_createProxyTracker(registration, &registration->proxyTracker);
		serviceTracker_open(registration->proxyTracker);
	}

	return status;
}

celix_status_t importRegistration_stopTracking(import_registration_t registration) {
	celix_status_t status = CELIX_SUCCESS;

	if (registration->proxyTracker != NULL) {
		serviceTracker_close(registration->proxyTracker);
	}

	return status;
}

celix_status_t importRegistration_createProxyTracker(import_registration_t registration, service_tracker_t *tracker) {
	celix_status_t status = CELIX_SUCCESS;

	service_tracker_customizer_t customizer = NULL;

	status = serviceTrackerCustomizer_create(registration->pool, registration, importRegistration_proxyAdding,
			importRegistration_proxyAdded, importRegistration_proxyModified, importRegistration_proxyRemoved, &customizer);

	if (status == CELIX_SUCCESS) {
		char *filter = apr_pstrcat(registration->pool, "(&(", OBJECTCLASS, "=", REMOTE_PROXY,
				")(proxy.interface=", registration->endpointDescription->service, "))", NULL);
		status = serviceTracker_createWithFilter(registration->pool, registration->context, filter, customizer, tracker);
	}

	return status;
}

celix_status_t importRegistration_proxyAdding(void * handle, SERVICE_REFERENCE reference, void **service) {
	celix_status_t status = CELIX_SUCCESS;
	import_registration_t registration = handle;

	bundleContext_getService(registration->context, reference, service);

	return status;
}

celix_status_t importRegistration_proxyAdded(void * handle, SERVICE_REFERENCE reference, void *service) {
	celix_status_t status = CELIX_SUCCESS;
	import_registration_t registration = handle;

	remote_proxy_service_t proxy = service;
	if (registration->proxy == NULL) {
		registration->reference = reference;
		registration->proxy = proxy;
		if (registration->endpointDescription != NULL) {
			proxy->setEndpointDescription(proxy->proxy, registration->endpointDescription);
		}
	}

	return status;
}

celix_status_t importRegistration_proxyModified(void * handle, SERVICE_REFERENCE reference, void *service) {
	celix_status_t status = CELIX_SUCCESS;
	import_registration_t registration = handle;

	//remote_proxy_service_t endpoint = service;
	//if (registration->endpoint != NULL) {
	//	registration->endpoint = endpoint;
	//	endpoint->setServiceReference(endpoint->endpoint, registration->reference);
	//}

	return status;
}

celix_status_t importRegistration_proxyRemoved(void * handle, SERVICE_REFERENCE reference, void *service) {
	celix_status_t status = CELIX_SUCCESS;
	import_registration_t registration = handle;

	remote_proxy_service_t proxy = service;
	if (registration->proxy != NULL) {
		registration->reference = NULL;
		registration->proxy = NULL;
		proxy->setEndpointDescription(proxy->proxy, NULL);
	}

	return status;
}

celix_status_t importRegistration_open(import_registration_t registration) {
	celix_status_t status = CELIX_SUCCESS;

	char *bundleStore = NULL;
	bundleContext_getProperty(registration->context, BUNDLE_STORE_PROPERTY_NAME, &bundleStore);
	if (bundleStore == NULL) {
		bundleStore = DEFAULT_BUNDLE_STORE;
	}
	char *name = apr_pstrcat(registration->pool, bundleStore, "/", registration->endpointDescription->service, "_proxy.zip", NULL);
	status = bundleContext_installBundle(registration->context, name, &registration->bundle);
	if (status == CELIX_SUCCESS) {
		status = bundle_start(registration->bundle, 0);
		if (status == CELIX_SUCCESS) {
		}
	}

	return status;
}

celix_status_t importRegistration_close(import_registration_t registration) {
	celix_status_t status = CELIX_SUCCESS;

	importRegistration_stopTracking(registration);

	if (registration->bundle != NULL) {
		bundle_stop(registration->bundle, 0);
		bundle_uninstall(registration->bundle);
	}

	return status;
}

celix_status_t importRegistration_getException(import_registration_t registration) {
	celix_status_t status = CELIX_SUCCESS;
	return status;
}

celix_status_t importRegistration_getImportReference(import_registration_t registration, import_reference_t *reference) {
	celix_status_t status = CELIX_SUCCESS;

	if (registration->importReference == NULL) {
		registration->importReference = apr_palloc(registration->pool, sizeof(*registration->importReference));
		if (registration->importReference == NULL) {
			status = CELIX_ENOMEM;
		} else {
			registration->importReference->endpoint = registration->endpointDescription;
			registration->importReference->reference = registration->reference;
		}
	}

	*reference = registration->importReference;

	return status;
}

celix_status_t importRegistration_setEndpointDescription(import_registration_t registration, endpoint_description_t endpointDescription) {
	celix_status_t status = CELIX_SUCCESS;

	registration->endpointDescription = endpointDescription;
	if (registration->proxy != NULL) {
		registration->proxy->setEndpointDescription(registration->proxy->proxy, endpointDescription);
	}

	return status;
}